#include <HardwareSerial.h>

/* CONFIGURAÇÕES LORA */

// Pinos da Serial 2 do ESP32
#define RX2_PIN 16
#define TX2_PIN 17
#define M0 32
#define M1 33

#define LORA_STRING_METHOD 1
#define LORA_STRUCT_METHOD 2

#define LORA_WAY 1

// Usando a Serial 2 do ESP32
HardwareSerial LoRaSerial(2);
int sizeAllData = sizeof(allData);
int sizeSoloData = sizeof(soloData);

void setupTelemetry()
{
  // Modo de operação normal - transmite e recebe dados
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);

  // Configuração inicial do LoRa
  LoRaSerial.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  while (!LoRaSerial)
    ;

  println("LoRa conectado!");
}

void receiveStruct(SoloData *soloData)
{
  LoRaSerial.readBytes((char *)soloData, sizeSoloData);
}

void transmitString(const String &string)
{
  LoRaSerial.println(string);
}

void receiveString()
{
  solo_message = LoRaSerial.readStringUntil('\n');
}

void transmit()
{
  if (LORA_WAY == LORA_STRING_METHOD)
  {
    Serial.println("Transmitindo a mensagem de telemetria");
    Serial.println(telemetry_message);
    transmitString(telemetry_message);
    uint32_t start = micros();
    LoRaSerial.flush();
    uint32_t elapsed = micros() - start;

    Serial.printf("Tempo de flush: %lu us\n", elapsed);
  }
  // else if (LORA_WAY == LORA_STRUCT_METHOD)
  // {
  //   transmitStruct(&allData);
  // }
}

void receive()
{
  if (LORA_WAY == LORA_STRING_METHOD)
  {
    receiveString();
  }
  else if (LORA_WAY == LORA_STRUCT_METHOD)
  {
    receiveStruct(&soloData);
  }
}

bool hasSoloMessage()
{
  return LoRaSerial.available() > 0;
}