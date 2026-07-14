// Projeto Joliot - Avionica 1Km

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
  Serial.println(allData.data.parachute);

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

int package_counter = 0;

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
#include "debug.h"
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

const unsigned long telemetryInterval = 3000; // intervalo de 3 segundos
void enterLORAConfigMode()
{
  digitalWrite(M0, HIGH);
  digitalWrite(M1, HIGH);
  delay(50); // aguarda estabilizar
}

void exitLORAConfigMode()
{
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  delay(50);
}

void flash_up()
{
  digitalWrite(LED_ACTIVE, HIGH);
}

void flash_down() {
  digitalWrite(LED_ACTIVE, LOW);
}

void setup()
{
  Wire.begin();
  Wire.setClock(400000);

  sd_message.reserve(1500);
  telemetry_message.reserve(1500);

  Serial.begin(115200);
  Serial.println("-------------------------------------");
  Serial.println("------ Inicializando Sistema --------");
  Serial.println("-------------------------------------");

  setupComponents();
  getInitialAltitude();
  resetStructs();
  tripleBuzzerBip();

  pinMode(LED_ACTIVE, OUTPUT);

  enterLORAConfigMode();
  Serial.println("Setando a potência de transmissão do LORA para 21dbm");
  LoRaSerial.print("AT+POWER=3\r\n"); // exemplo: seta para 21 dBm
  unsigned long t0 = millis();
  while (millis() - t0 < 500)
  {
    if (LoRaSerial.available())
    {
      Serial.write(LoRaSerial.read());
    }
  }
  exitLORAConfigMode();

  Serial.printf("Reset reason: %d\n", esp_reset_reason());

  delay(1000);
}

void loop()
{
  int executionTime = millis() / 1000;

  debugPacketData();
  getSensorsMeasures();
  // Serial.println("IsDropping: " + String(isDropping));

  allData.data.time = millis() / 1000.0;

  checkApogee();
  saveMessages();

  println(telemetry_message);
  // debugTelemetryMessage(telemetry_message);

  if (ENABLE_SD)
  {
    verifySD();
    if (setupSDFlag)
    {
      writeOnSD(sd_message);
    }
    else
    {
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
  delay(200);
}
