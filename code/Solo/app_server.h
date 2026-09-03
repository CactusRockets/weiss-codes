#pragma once
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

const char *ssid = "CARDOSO_2G";
const char *password = "aonet123456789";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
bool wifiConnected = false;
bool mdnsStarted = false;
uint32_t lastWifiAttempt = 0;
uint32_t lastMdnsAttempt = 0;

void onWsEvent(AsyncWebSocket *, AsyncWebSocketClient *, AwsEventType type,
               void *, uint8_t *, size_t) {
  if (type == WS_EVT_CONNECT) Serial.println("Cliente WebSocket conectado.");
  if (type == WS_EVT_DISCONNECT) Serial.println("Cliente WebSocket desconectado.");
}

void setupServer() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  lastWifiAttempt = millis();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Solo: telemetria V2, WebSocket /ws");
  });
  server.begin();
  Serial.println("Servidor HTTP iniciado; recepcao LoRa independe do WiFi.");
}

void maintainWiFi() {
  const uint32_t now = millis();
  if (WiFi.status() == WL_CONNECTED) {
    if (!wifiConnected) {
      wifiConnected = true;
      Serial.print("WiFi conectado. IP: ");
      Serial.println(WiFi.localIP());
      lastMdnsAttempt = now - 10000;
    }
    if (!mdnsStarted && static_cast<uint32_t>(now - lastMdnsAttempt) >= 10000) {
      lastMdnsAttempt = now;
      mdnsStarted = MDNS.begin("esp32");
      if (mdnsStarted) {
        MDNS.addService("http", "tcp", 80);
        Serial.println("mDNS configurado: esp32.local");
      }
    }
  } else {
    if (wifiConnected) {
      wifiConnected = false;
      if (mdnsStarted) MDNS.end();
      mdnsStarted = false;
      Serial.println("WiFi desconectado; LoRa continua recebendo.");
    }
    if (static_cast<uint32_t>(now - lastWifiAttempt) >= 15000) {
      lastWifiAttempt = now;
      WiFi.reconnect();
    }
  }
}
