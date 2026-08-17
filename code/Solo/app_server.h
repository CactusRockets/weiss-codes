#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

const char *ssid = "BrenoNet";
const char *password = "breno123";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(
    AsyncWebSocket *server,
    AsyncWebSocketClient *client,
    AwsEventType type,
    void *arg,
    uint8_t *data,
    size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.println("Cliente conectado");
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.println("Cliente desconectado");
  }
  else if (type == WS_EVT_DATA)
  {
    Serial.print("Dados recebidos: ");
    Serial.write(data, len);
    Serial.println();
  }
}

void setupServer()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin("esp32"))
  {
    Serial.println("Erro ao configurar mDNS");
    return;
  }
  Serial.println("mDNS configurado como esp32.local");

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Adicione uma rota básica para verificar a conexão HTTP
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Servidor ESP32 funcionando"); });
}
