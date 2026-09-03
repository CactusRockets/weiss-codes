// Projeto Weiss - Avionica 1Km

#include "esp_system.h"

// import support libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>

#define ENABLE_SERIAL true
#define ENABLE_BUZZER true
#define ENABLE_BMP true
#define ENABLE_MPU true
#define ENABLE_SKIBS true
#define ENABLE_SD true
#define ENABLE_TELEMETRY true
#define ENABLE_GPS true

#define LED_ACTIVE 2

// Periodo alvo do loop principal, em milissegundos
#define LOOP_PERIOD_MS 200
// Intervalo do debug serial, em milissegundos
#define DEBUG_INTERVAL_MS 1000

struct AvionicData
{
  float time;
  int parachute;
};

struct BmpData
{
  float temperature;
  float pressure;
  float altitude;
};

struct ImuData
{
  float accelX;
  float accelY;
  float accelZ;
  float quaternion_w;
  float quaternion_x;
  float quaternion_y;
  float quaternion_z;
};

struct GpsData
{
  String date;
  String hour;
  double latitude, longitude;
};

struct PacketData
{
  AvionicData data;
  BmpData bmpData;
  ImuData imuData;
  GpsData gpsData;
  int parachute;
};
struct SoloData
{
  int openParachute;
};

PacketData allData;
SoloData soloData;

void debugPacketData()
{
  Serial.println("==== Dados registrados no momento ====");

  Serial.println("[AvionicData]");
  Serial.print(" - Time: ");
  Serial.println(allData.data.time);
  Serial.print(" - Parachute: ");
  Serial.println(allData.parachute);

  Serial.println("[BmpData]");
  Serial.print(" - Temperature: ");
  Serial.println(allData.bmpData.temperature);
  Serial.print(" - Pressure: ");
  Serial.println(allData.bmpData.pressure);
  Serial.print(" - Altitude: ");
  Serial.println(allData.bmpData.altitude);

  Serial.println("[ImuData]");
  Serial.print(" - AccelX: ");
  Serial.println(allData.imuData.accelX);
  Serial.print(" - AccelY: ");
  Serial.println(allData.imuData.accelY);
  Serial.print(" - AccelZ: ");
  Serial.println(allData.imuData.accelZ);
  Serial.print(" - Quaternion W: ");
  Serial.println(allData.imuData.quaternion_w);
  Serial.print(" - Quaternion X: ");
  Serial.println(allData.imuData.quaternion_x);
  Serial.print(" - Quaternion Y: ");
  Serial.println(allData.imuData.quaternion_y);
  Serial.print(" - Quaternion Z: ");
  Serial.println(allData.imuData.quaternion_z);

  Serial.println("[GpsData]");
  Serial.print(" - Date: ");
  Serial.println(allData.gpsData.date);
  Serial.print(" - Hour: ");
  Serial.println(allData.gpsData.hour);
  Serial.print(" - Latitude: ");
  Serial.println(allData.gpsData.latitude, 6); // 6 casas decimais p/ GPS
  Serial.print(" - Longitude: ");
  Serial.println(allData.gpsData.longitude, 6);

  Serial.println("[SoloData]");
  Serial.print(" - OpenParachute: ");
  Serial.println(soloData.openParachute);

  Serial.println("====================================");
}

String telemetry_message = "";
String sd_message = "";
String solo_message = "";

bool isBeeping = false;

bool setupSDFlag = false;
bool setupMPUFlag = false;
bool setupBMPFlag = false;
bool setupGPSFlag = false;

uint32_t package_counter = 0;

float initial_altitude;

// import external files
#include "serial.h"
#include "buzzer.h"
#include "telemetry.h"
#include "moduleSD.h"
#include "bmp.h"
#include "imu.h"
#include "gps.h"
#include "parachute.h"
#include "setup.h"
#include "messages.h"

void setupComponents();
void getSensorsMeasures();
void beepIntermitating();
void activateParachutes();
void resetStructs();
void checkApogee();
void saveMessages();

// Variáveis para controle de tempo
unsigned long lastTelemetryTime = 0;
unsigned long lastDebugTime = 0;

// Diagnostico de timing do loop
unsigned long worstLoopTime = 0;

const unsigned long telemetryInterval = 3000; // intervalo de 3 segundos

void flash_up()
{
  digitalWrite(LED_ACTIVE, HIGH);
}

void flash_down()
{
  digitalWrite(LED_ACTIVE, LOW);
}

void setup()
{
  Wire.begin();
  Wire.setClock(400000);

  sd_message.reserve(1500);
  telemetry_message.reserve(1500);

  Serial.begin(115200);
  Serial.println("[FW] AVIONICA-TELEMETRY-V2");
  Serial.println("-------------------------------------");
  Serial.println("------ Inicializando Sistema --------");
  Serial.println("-------------------------------------");

  tripleBuzzerBip();
  setupComponents();
  getInitialAltitude();
  resetStructs();

  pinMode(LED_ACTIVE, OUTPUT);

  if (ENABLE_TELEMETRY)
  {
    // Mantem a configuracao persistida no E32. O significado dos comandos
    // depende da revisao; nas revisoes com AT, POWER=3 significa 10 dBm.
    Serial.println("[LoRa TX] Modo normal desde o setup; parametros gravados preservados.");
  }

  Serial.printf("Reset reason: %d\n", esp_reset_reason());

  delay(1000);
}

void loop()
{
  unsigned long loopStart = millis();

  // ===== PRIORIDADE: leitura dos sensores e decisao de apogeu =====
  getSensorsMeasures();

  allData.data.time = millis() / 1000.0;

  checkApogee();
  // ================================================================

  saveMessages();

  if (ENABLE_SD)
  {
    if (setupSDFlag)
    {
      writeOnSD(sd_message);
    }
    else
    {
      // Só tenta reinicializar quando o SD já falhou.
      // Rodar verifySD() todo loop pode travar centenas de ms.
      verifySD();
      wrapperSetupSD();
    }
  }

  if (ENABLE_TELEMETRY)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - lastTelemetryTime >= telemetryInterval)
    {
      lastTelemetryTime = currentMillis;

      transmit();
      if (hasSoloMessage())
      {
        receive();
      }
    }
  }

  // Debug serial limitado a 1 Hz para nao bloquear o loop
  if (ENABLE_SERIAL && (millis() - lastDebugTime >= DEBUG_INTERVAL_MS))
  {
    lastDebugTime = millis();

    debugPacketData();
    Serial.println("IsDropping: " + String(isDropping));
    Serial.println("Altitude atual: " + String(altitudeAtual) +
                   " | Maxima: " + String(highestAltitude));
    Serial.println("Pior loop (ms): " + String(worstLoopTime));
  }

  // Delay compensado: mantem o periodo do loop constante
  unsigned long elapsed = millis() - loopStart;

  if (elapsed > worstLoopTime)
  {
    worstLoopTime = elapsed;
  }

  // Reporta na hora qualquer loop lento, com timestamp.
  // Serve para distinguir um evento unico de boot de algo recorrente.
  if (elapsed > 300)
  {
    Serial.print("!!! LOOP LENTO: ");
    Serial.print(elapsed);
    Serial.print(" ms | em t = ");
    Serial.print(millis() / 1000.0);
    Serial.println(" s");
  }

  if (elapsed < LOOP_PERIOD_MS)
  {
    delay(LOOP_PERIOD_MS - elapsed);
  }
}
