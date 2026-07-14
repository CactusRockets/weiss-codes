#include <TinyGPS++.h>
#include <HardwareSerial.h>

// Pinos da Serial 1 do ESP32
#define RX1_PIN 15
#define TX1_PIN 4

TinyGPSPlus gps;
// Usando a Serial 1 do ESP32
HardwareSerial gpsSerial(1);

String gpsData;
int gps_available;

double latitude = 0, longitude = 0;
int ano, mes, dia, hora, minuto, segundo;

void verifyGPS()
{
  gpsSerial.begin(9600, SERIAL_8N1, RX1_PIN, TX1_PIN);
  setupGPSFlag = !!gpsSerial;
}

void setupGPS()
{
  gpsSerial.begin(9600, SERIAL_8N1, RX1_PIN, TX1_PIN);
  println("GPS conectado!");
}

void getGPSData()
{
  while (true)
  {
    gps_available = gpsSerial.available();

    if (gps_available <= 0)
    {
      break;
    }
    // Processa os dados recebidos do GPS
    gps.encode(gpsSerial.read());

    // Se houver uma nova atualização de posição
    if (gps.location.isValid())
    {
      latitude = gps.location.lat();
      longitude = gps.location.lng();
    }

    // Se houver atualização da data
    if (gps.date.isValid())
    {
      dia = gps.date.day();
      mes = gps.date.month();
      ano = gps.date.year();
    }

    // Se houver atualização do horário
    if (gps.time.isValid())
    {
      hora = gps.time.hour();
      minuto = gps.time.minute();
      segundo = gps.time.second();
    }

    // // Se houver atualização da velocidade
    // if (gps.speed.isValid()) {
    //     Serial.print("Velocidade (km/h): ");
    //     Serial.println(gps.speed.kmph());
    // }
  }
}

void saveGPSData()
{
  String date = String(dia) + "/" + String(mes) + "/" + String(ano);
  String hour = String(hora) + ":" + String(minuto) + ":" + String(segundo);

  allData.gpsData.hour = hour;
  allData.gpsData.date = date;
  allData.gpsData.latitude = latitude;
  allData.gpsData.longitude = longitude;
}

void printData()
{
  println(gpsData);

  print("Latitude: ");
  println(latitude, 6);
  print("Longitude: ");
  println(longitude, 6);

  print("Data: ");
  print(dia);
  print("/");
  print(mes);
  print("/");
  println(ano);
}

void updateGPSData()
{
  getGPSData();
  saveGPSData();
  // printData();
}
