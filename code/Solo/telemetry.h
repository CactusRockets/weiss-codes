#pragma once
#include <HardwareSerial.h>
#include "auxiliary.h"

// Mesma pinagem do receptor generico informado: M0=19, M1=18, AUX=4.
#define RX2_PIN 16
#define TX2_PIN 17
#define M0 19
#define M1 18
#define AUX_PIN 4

HardwareSerial LoRaSerial(2);
SoloProtocol::LineReader loraReader;
uint32_t receivedBytes = 0;
uint32_t validPackets = 0;
uint32_t invalidPackets = 0;
uint32_t lastValidPacketAt = 0;

void setupTelemetry() {
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(AUX_PIN, INPUT); // GPIO4 e AUX; nao configurar como botao com pull-up.
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  LoRaSerial.setRxBufferSize(2048);
  LoRaSerial.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  delay(2);
  Serial.printf("[LoRa] V2 | UART2 RX=%d TX=%d | 9600 8N1 | M0=%d M1=%d AUX=%d\n",
                RX2_PIN, TX2_PIN, digitalRead(M0), digitalRead(M1), digitalRead(AUX_PIN));
  Serial.println("[LoRa] AUX alto nao confirma enlace RF; aguardando pacotes validos.");
}

// Retorna um pacote completo por chamada; o loop consome todos os disponiveis.
bool receiveTelemetry(DataFlight &data) {
  loraReader.expire(millis());
  while (LoRaSerial.available() > 0) {
    const int byteRead = LoRaSerial.read();
    if (byteRead < 0) break;
    ++receivedBytes;
    if (!loraReader.push(static_cast<char>(byteRead), millis())) continue;
    const char *error = nullptr;
    const bool valid = SoloProtocol::decode(loraReader.frame, loraReader.length, data, error);
    loraReader.consumed();
    if (valid) {
      ++validPackets;
      lastValidPacketAt = millis();
      return true;
    }
    ++invalidPackets;
    Serial.printf("[LoRa] Pacote descartado: %s\n", error);
  }
  return false;
}

void printLoRaStatus() {
  static uint32_t lastStatus = 0;
  static uint32_t previousBytes = 0;
  const uint32_t now = millis();
  if (static_cast<uint32_t>(now - lastStatus) < 2000) return;
  const uint32_t delta = receivedBytes - previousBytes;
  Serial.printf("[LoRa] %s | AUX=%d M0=%d M1=%d | bytes=%lu (+%lu) | buffer=%u | validos=%lu invalidos=%lu incompletos=%lu longos=%lu\n",
                delta ? "DADOS NA UART" : "SEM DADOS NA UART", digitalRead(AUX_PIN),
                digitalRead(M0), digitalRead(M1), static_cast<unsigned long>(receivedBytes),
                static_cast<unsigned long>(delta), static_cast<unsigned>(loraReader.length),
                static_cast<unsigned long>(validPackets), static_cast<unsigned long>(invalidPackets),
                static_cast<unsigned long>(loraReader.incomplete), static_cast<unsigned long>(loraReader.overflow));
  if (validPackets) Serial.printf("[LoRa] Ultimo pacote valido ha %lu ms\n", static_cast<unsigned long>(now - lastValidPacketAt));
  lastStatus = now;
  previousBytes = receivedBytes;
}
