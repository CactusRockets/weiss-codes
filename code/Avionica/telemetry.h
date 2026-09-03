#include <HardwareSerial.h>

/* CONFIGURAÇÕES LORA */

// Pinos da Serial 2 do ESP32
#define RX2_PIN 16
#define TX2_PIN 17
#define M0 22
#define M1 21

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
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);

  // Configuração inicial do LoRa
  LoRaSerial.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  delay(50);
  Serial.printf("[LoRa TX] UART2 RX=%d TX=%d baud=%lu | M0(GPIO%d)=%d M1(GPIO%d)=%d\n",
                RX2_PIN, TX2_PIN, static_cast<unsigned long>(LoRaSerial.baudRate()),
                M0, digitalRead(M0), M1, digitalRead(M1));
  Serial.println("[LoRa TX] UART inicializada; isso nao confirma a presenca do radio.");
}

void receiveStruct(SoloData *soloData)
{
  LoRaSerial.readBytes((char *)soloData, sizeSoloData);
}

size_t transmitString(const String &string)
{
  return LoRaSerial.println(string);
}

void receiveString()
{
  solo_message = LoRaSerial.readStringUntil('\n');
}

void transmit()
{
  if (telemetry_message.isEmpty()) return;
  if (LORA_WAY == LORA_STRING_METHOD)
  {
    Serial.println("Transmitindo a mensagem de telemetria");
    Serial.println(telemetry_message);
    const int mode0 = digitalRead(M0);
    const int mode1 = digitalRead(M1);
    if (mode0 != LOW || mode1 != LOW)
    {
      Serial.printf("[LoRa TX] Envio cancelado: M0=%d M1=%d; esperado 0/0.\n", mode0, mode1);
      return;
    }
    const size_t expectedBytes = telemetry_message.length() + 2; // CR + LF
    const size_t writtenBytes = transmitString(telemetry_message);
    uint32_t start = micros();
    LoRaSerial.flush();
    uint32_t elapsed = micros() - start;

    Serial.printf("[LoRa TX] dados=%u caracteres | UART aceitou=%u/%u bytes | M0=%d M1=%d | flush=%lu us\n",
                  static_cast<unsigned int>(telemetry_message.length()),
                  static_cast<unsigned int>(writtenBytes), static_cast<unsigned int>(expectedBytes),
                  mode0, mode1, static_cast<unsigned long>(elapsed));
    if (writtenBytes != expectedBytes)
      Serial.println("[LoRa TX] ERRO: escrita incompleta na UART.");
    Serial.println("[LoRa TX] UART finalizada; aguardamos confirmacao no monitor do Solo.");
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
