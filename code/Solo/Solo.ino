#include <Arduino.h>
#include "model.h"
#include "auxiliary.h"
#include "telemetry.h"
#include "app_server.h"

DataFlight data;

void setup() {
  Serial.begin(115200);
  Serial.println("[FW] SOLO-TELEMETRY-V2");
  setupTelemetry();
  setupServer();
}

void loop() {
  while (receiveTelemetry(data)) {
    JsonDocument document;
    SoloProtocol::toJson(data, document);
    String json;
    serializeJson(document, json);
    Serial.printf("[LoRa] Pacote V2 #%lu valido | WS=%u\n",
                  static_cast<unsigned long>(data.sequence), static_cast<unsigned>(ws.count()));
    Serial.println(json);
    if (ws.count() > 0) ws.textAll(json);
  }
  printLoRaStatus();
  maintainWiFi();
  static uint32_t lastCleanup = 0;
  if (static_cast<uint32_t>(millis() - lastCleanup) >= 1000) {
    lastCleanup = millis();
    ws.cleanupClients();
  }
  delay(1);
}
