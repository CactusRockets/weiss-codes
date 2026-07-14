#include <SPI.h>
#include <Wire.h>
#include "debug.h"
#include "model.h"
#include "server.h"
#include "telemetry.h"

#define BUTTON_PIN 4
#define EXPECTED_PACKET_SIZE 31

int valueButton;
String solo_message = "";
DataFlight data = DataFlight();

void setup()
{
  Serial.begin(115200);
  Serial.println("Serial inicializada!");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);

  setupServer();
  server.begin();
  setupTelemetry();
}

void loop()
{
  // telemetry_message.clear();

   if(!LoRaSerial.available()){
    // Serial.println("Lora não está disponível");
   }
   delay(500);

  if (LoRaSerial.available() > 0)
  {

    Serial.println("LoRa available!");

    receiveString();

    // PADRÃO DE MENSAGEM TRANSMITIDA - CUIDADO AO MUDAR ISSO
    // Numero do pacote -> primeiros 5 dígitos sem sinal
    // Altitude -> próximos 7 dígitos COM sinal
    // Aceleração em Z -> próximos 5 dígitos COM sinal
    // Quaternion W -> próximos 4 dígitos COM sinal
    // Quaternion X -> próximos 4 dígitos COM sinal
    // Quaternion Y -> próximos 4 dígitos COM sinal
    // Quartenion Z -> próximos 4 dígitos COM sinal
    // CUIDADO - Paraquedas - próximo dígito, no final do quaternion Z, é uma flag de ativação do paraquedas
    // Latitude -> próximos 6 dígitos COM sinal
    // Longitude -> próximos 6 dígitos COM sinal

    debugMessage(telemetry_message);
    // debugMessageString(telemetry_message);

    Serial.println("Mensagem Recebida: ");
    Serial.println(telemetry_message);
    Serial.print("Número de Conexões: ");
    Serial.println(ws.count());

    if (ws.count() > 0)
    {
      StaticJsonDocument<500> doc;

      data = DataFlight(telemetry_message);

      doc["package"] = data.getPackage();
      doc["altitude"] = data.getAltitude();
      doc["accelerationZ"] = data.getAccelerationZ();
      doc["quaternion_w"] = data.getQuaternionW();
      doc["quaternion_x"] = data.getQuaternionX();
      doc["quaternion_y"] = data.getQuaternionY();
      doc["quaternion_z"] = data.getQuaternionZ();
      doc["skib1"] = data.getSkib1();
      doc["skib2"] = data.getSkib2();
      doc["latitude"] = data.getLatitude();
      doc["longitude"] = data.getLongitude();
      doc["velocity"] = data.getVelocity();
      doc["velocityX"] = data.getVelocityX();
      doc["velocityY"] = data.getVelocityY();
      doc["velocityZ"] = data.getVelocityZ();
      doc["maximumVelocity"] = data.getMaximumVelocity();
      doc["maximumAcceleration"] = data.getMaximumAcceleration();
      doc["acceleration"] = data.getAcceleration();
      doc["accelerationX"] = data.getAccelerationX();
      doc["accelerationY"] = data.getAccelerationY();
      doc["maximumAltitude"] = data.getMaximumAltitude();

      counter++;
      tempo = tempo + 0.01;
      if (counter > 31)
        counter = 0;

      String json;
      serializeJson(doc, json);
      ws.textAll(json);
      Serial.print("Enviando dados: ");
      Serial.println(json);
    }
    delay(20);
  }
}
