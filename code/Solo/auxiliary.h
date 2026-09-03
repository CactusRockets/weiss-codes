#pragma once
#include "model.h"
#include <ArduinoJson.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

namespace SoloProtocol {
constexpr size_t CAPACITY = 384;
constexpr uint32_t RX_TIMEOUT_MS = 3000;
struct Field { const char *name; double DataFlight::*member; };
static const Field FIELDS[] = {
  {"maximumAltitude", &DataFlight::maximumAltitude},
  {"altitude", &DataFlight::altitude},
  {"maximumVelocity", &DataFlight::maximumVelocity},
  {"velocity", &DataFlight::velocity},
  {"maximumAcceleration", &DataFlight::maximumAcceleration},
  {"acceleration", &DataFlight::acceleration},
  {"velocityX", &DataFlight::velocityX},
  {"velocityY", &DataFlight::velocityY},
  {"velocityZ", &DataFlight::velocityZ},
  {"accelerationX", &DataFlight::accelerationX},
  {"accelerationY", &DataFlight::accelerationY},
  {"accelerationZ", &DataFlight::accelerationZ},
  {"quaternion_w", &DataFlight::quaternion_w},
  {"quaternion_x", &DataFlight::quaternion_x},
  {"quaternion_y", &DataFlight::quaternion_y},
  {"quaternion_z", &DataFlight::quaternion_z},
  {"latitude", &DataFlight::latitude},
  {"longitude", &DataFlight::longitude}
};
inline uint16_t crc16(const char *text, size_t length) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < length; ++i) {
    crc ^= static_cast<uint16_t>(static_cast<unsigned char>(text[i])) << 8;
    for (int bit = 0; bit < 8; ++bit)
      crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021)
                           : static_cast<uint16_t>(crc << 1);
  }
  return crc;
}

inline bool decimal(const char *text, double &value) {
  if (strcmp(text, "~") == 0) { value = NAN; return true; }
  // Aceita somente decimal, incluindo notacao cientifica. Nao aceita nan/inf/hex.
  const char *p = text;
  if (*p == '-' || *p == '+') ++p;
  bool digit = false;
  while (*p >= '0' && *p <= '9') { digit = true; ++p; }
  if (*p == '.') { ++p; while (*p >= '0' && *p <= '9') { digit = true; ++p; } }
  if (!digit) return false;
  if (*p == 'e' || *p == 'E') {
    ++p;
    if (*p == '-' || *p == '+') ++p;
    const char *first = p;
    while (*p >= '0' && *p <= '9') ++p;
    if (first == p) return false;
  }
  if (*p) return false;
  errno = 0;
  char *end = nullptr;
  value = strtod(text, &end);
  return errno != ERANGE && *end == '\0' && isfinite(value);
}

inline bool decode(const char *frame, size_t length, DataFlight &result, const char *&error) {
  error = "formato/versao";
  if (!frame || length < 12 || length >= CAPACITY) return false;
  if (strncmp(frame, "@2|", 3) != 0 || frame[length - 5] != '*') return false;
  uint16_t expected = 0;
  for (size_t i = length - 4; i < length; ++i) {
    const char c = frame[i];
    const int nibble = (c >= '0' && c <= '9') ? c - '0' :
                       (c >= 'A' && c <= 'F') ? c - 'A' + 10 :
                       (c >= 'a' && c <= 'f') ? c - 'a' + 10 : -1;
    if (nibble < 0) return false;
    expected = static_cast<uint16_t>((expected << 4) | nibble);
  }
  error = "CRC";
  if (crc16(frame, length - 5) != expected) return false;
  char body[CAPACITY];
  memcpy(body, frame, length - 5);
  body[length - 5] = '\0';
  char *tokens[22]; // versao + sequencia + 18 numeros + 2 flags
  size_t count = 1;
  tokens[0] = body;
  for (size_t i = 0; i < length - 5; ++i) {
    if (body[i] == '\0') { error = "NUL no pacote"; return false; }
    if (body[i] == '|') {
      if (count == 22) { error = "campos extras"; return false; }
      body[i] = '\0';
      tokens[count++] = body + i + 1;
    }
  }
  error = "quantidade de campos";
  if (count != 22) return false;
  DataFlight candidate;
  error = "sequencia";
  if (!*tokens[1]) return false;
  uint32_t sequence = 0;
  for (const char *p = tokens[1]; *p; ++p) {
    if (*p < '0' || *p > '9') return false;
    const unsigned digit = *p - '0';
    if (sequence > (UINT32_MAX - digit) / 10) return false;
    sequence = sequence * 10 + digit;
  }
  candidate.sequence = sequence;
  error = "numero invalido";
  for (size_t i = 0; i < 18; ++i)
    if (!decimal(tokens[i + 2], candidate.*(FIELDS[i].member))) return false;
  error = "coordenadas fora da faixa";
  if ((!isnan(candidate.latitude) && fabs(candidate.latitude) > 90) ||
      (!isnan(candidate.longitude) && fabs(candidate.longitude) > 180)) return false;
  error = "skibs: esperado 0 ou 1";
  if ((strcmp(tokens[20], "0") != 0 && strcmp(tokens[20], "1") != 0) ||
      (strcmp(tokens[21], "0") != 0 && strcmp(tokens[21], "1") != 0)) return false;
  candidate.skib1 = tokens[20][0] == '1';
  candidate.skib2 = tokens[21][0] == '1';
  result = candidate; // Atualiza apenas depois de validar todo o pacote.
  error = nullptr;
  return true;
}

inline void toJson(const DataFlight &data, JsonDocument &document) {
  document.clear();
  for (const Field &field : FIELDS) {
    const double value = data.*(field.member);
    if (isfinite(value)) document[field.name] = value;
    else document[field.name] = nullptr;
  }
  document["skibs"]["skib1"] = data.skib1;
  document["skibs"]["skib2"] = data.skib2;
}

// Montagem incremental: tolera fragmentos RF, CRLF, varias linhas e perda de sincronismo.
class LineReader {
 public:
  char frame[CAPACITY] = {};
  size_t length = 0;
  uint32_t incomplete = 0;
  uint32_t overflow = 0;
  bool push(char c, uint32_t now) {
    expire(now);
    if (c == '@') {
      if (length) ++incomplete;
      length = 0;
      active = true;
    }
    if (!active) return false;
    lastByte = now;
    if (c == '\n') {
      if (length && frame[length - 1] == '\r') --length;
      frame[length] = '\0';
      active = false;
      return true; // Consumidor processa frame antes do proximo push.
    }
    if (length >= CAPACITY - 1) {
      ++overflow;
      length = 0;
      active = false;
      return false;
    }
    frame[length++] = c;
    return false;
  }
  void consumed() { length = 0; }
  void expire(uint32_t now) {
    if (active && static_cast<uint32_t>(now - lastByte) >= RX_TIMEOUT_MS) {
      ++incomplete;
      length = 0;
      active = false;
    }
  }
 private:
  uint32_t lastByte = 0;
  bool active = false;
};
} // namespace SoloProtocol
