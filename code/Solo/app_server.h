#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

const char *ssid = "CARDOSO_2G";
const char *password = "aonet123456789";

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
  Serial.flush();

  Serial.println("Iniciando mDNS...");
  Serial.flush();
  if (!MDNS.begin("esp32"))
  {
    // Falha no mDNS nao impede o servidor de funcionar pelo IP,
    // por isso nao abortamos mais o setup aqui.
    Serial.println("Erro ao configurar mDNS (seguindo mesmo assim)");
  }
  else
  {
    Serial.println("mDNS configurado como esp32.local");
  }
  Serial.flush();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Adicione uma rota básica para verificar a conexão HTTP
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", "Servidor ESP32 funcionando"); });
}
