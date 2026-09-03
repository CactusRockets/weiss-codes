#pragma once
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

namespace TelemetryV2 {
constexpr size_t CAPACITY = 384; // Inclui o terminador NUL; CRLF e adicionado pela UART.
struct Packet {
  uint32_t sequence = 0;
  double maximumAltitude = NAN;
  double altitude = NAN;
  double maximumVelocity = NAN;
  double velocity = NAN;
  double maximumAcceleration = NAN;
  double acceleration = NAN;
  double velocityX = NAN;
  double velocityY = NAN;
  double velocityZ = NAN;
  double accelerationX = NAN;
  double accelerationY = NAN;
  double accelerationZ = NAN;
  double quaternion_w = NAN;
  double quaternion_x = NAN;
  double quaternion_y = NAN;
  double quaternion_z = NAN;
  double latitude = NAN;
  double longitude = NAN;
  bool skib1 = false;
  bool skib2 = false;
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
// @2|sequencia|18 valores na ordem do JSON|skib1|skib2*CRC16
// '~' representa uma medicao indisponivel, convertida para null no Solo.
inline bool encode(const Packet &packet, char *output, size_t capacity) {
  if (!output || capacity == 0) return false;
  output[0] = '\0';
  const double values[] = {
    packet.maximumAltitude,
    packet.altitude,
    packet.maximumVelocity,
    packet.velocity,
    packet.maximumAcceleration,
    packet.acceleration,
    packet.velocityX,
    packet.velocityY,
    packet.velocityZ,
    packet.accelerationX,
    packet.accelerationY,
    packet.accelerationZ,
    packet.quaternion_w,
    packet.quaternion_x,
    packet.quaternion_y,
    packet.quaternion_z,
    packet.latitude,
    packet.longitude
  };
  char frame[CAPACITY];
  int count = snprintf(frame, sizeof(frame), "@2|%lu", static_cast<unsigned long>(packet.sequence));
  if (count < 0 || static_cast<size_t>(count) >= sizeof(frame)) return false;
  size_t length = static_cast<size_t>(count);
  for (double value : values) {
    if (isinf(value)) return false;
    count = isnan(value) ? snprintf(frame + length, sizeof(frame) - length, "|~")
                        : snprintf(frame + length, sizeof(frame) - length, "|%.9g", value);
    if (count < 0 || static_cast<size_t>(count) >= sizeof(frame) - length) return false;
    length += static_cast<size_t>(count);
  }
  count = snprintf(frame + length, sizeof(frame) - length, "|%d|%d", packet.skib1, packet.skib2);
  if (count < 0 || static_cast<size_t>(count) >= sizeof(frame) - length) return false;
  length += static_cast<size_t>(count);
  const uint16_t checksum = crc16(frame, length);
  count = snprintf(frame + length, sizeof(frame) - length, "*%04X", checksum);
  if (count != 5 || length + 5 >= sizeof(frame)) return false;
  length += 5;
  if (length + 1 > capacity) return false;
  for (size_t i = 0; i <= length; ++i) output[i] = frame[i];
  return true;
}
} // namespace TelemetryV2
