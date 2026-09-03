#pragma once
#include "telemetry_packet.h"
#include "telemetry_math.h"

TelemetryV2::Packet flightTelemetry;
constexpr uint32_t GPS_MAX_AGE_MS = 10000;

void updateFlightTelemetry() {
  // Executa a cada leitura de sensores; os maximos sobrevivem entre transmissoes.
  TelemetryV2::Packet current;
  current.sequence = package_counter;
  current.maximumAltitude = flightTelemetry.maximumAltitude;
  current.maximumVelocity = flightTelemetry.maximumVelocity;
  current.maximumAcceleration = flightTelemetry.maximumAcceleration;

  if (bmpSampleValid) {
    current.altitude = allData.bmpData.altitude;
    FlightMath::updateMaximum(highestAltitude, current.maximumAltitude);
    FlightMath::updateMaximum(current.altitude, current.maximumAltitude);
  }
  if (imuSampleValid) {
    current.quaternion_w = allData.imuData.quaternion_w;
    current.quaternion_x = allData.imuData.quaternion_x;
    current.quaternion_y = allData.imuData.quaternion_y;
    current.quaternion_z = allData.imuData.quaternion_z;
    FlightMath::Vector a;
    if (FlightMath::linearAcceleration(IMUAccel.accelX, IMUAccel.accelY, IMUAccel.accelZ,
          current.quaternion_w, current.quaternion_x, current.quaternion_y, current.quaternion_z, a)) {
      current.accelerationX = a.x;
      current.accelerationY = a.y;
      current.accelerationZ = kalmanSampleValid ? verticalAcceleration : a.z;
      current.acceleration = FlightMath::magnitude(current.accelerationX, current.accelerationY,
                                                    current.accelerationZ);
      FlightMath::updateMaximum(current.acceleration, current.maximumAcceleration);
    }
  }
  if (kalmanSampleValid) current.velocityZ = verticalVelocity; // K.x(1), m/s
  if (ENABLE_GPS && gps.location.isValid() && gps.location.age() <= GPS_MAX_AGE_MS) {
    current.latitude = allData.gpsData.latitude;
    current.longitude = allData.gpsData.longitude;
  }
  if (ENABLE_GPS && gps.speed.isValid() && gps.speed.age() <= GPS_MAX_AGE_MS &&
      gps.course.isValid() && gps.course.age() <= GPS_MAX_AGE_MS) {
    const FlightMath::Vector v = FlightMath::gpsVelocity(gps.speed.mps(), gps.course.deg());
    current.velocityX = v.x;
    current.velocityY = v.y;
  }
  current.velocity = FlightMath::magnitude(current.velocityX, current.velocityY, current.velocityZ);
  FlightMath::updateMaximum(current.velocity, current.maximumVelocity);

  // Flags historicas de acionamento; nao representam continuidade eletrica dos skibs.
  current.skib1 = flightTelemetry.skib1 || parachute1Activated || allData.parachute > 0;
  current.skib2 = flightTelemetry.skib2 || allData.parachute > 1;
  if (ENABLE_SKIBS) {
    current.skib1 = current.skib1 || digitalRead(SKIB1) == HIGH;
    current.skib2 = current.skib2 || digitalRead(SKIB2) == HIGH;
  }
  flightTelemetry = current;
}

String telemetryMessage() {
  char frame[TelemetryV2::CAPACITY];
  if (!TelemetryV2::encode(flightTelemetry, frame, sizeof(frame))) {
    Serial.println("[LoRa TX] Pacote V2 invalido ou sem espaco; envio cancelado.");
    return String();
  }
  return String(frame);
}

String sdMessage()
{
  String sdMessage =
      "--------------------------------\nData n°" +
      String(package_counter) + "\n" +
      "Horário: " + allData.gpsData.hour +  "\n" +
      "Data: " + allData.gpsData.date + "\n" +
      "Temperatura: " + String(allData.bmpData.temperature, 2) + "\n" +
      "Pressão: " + String(allData.bmpData.pressure, 2) + "\n" +
      "Altitude: " + String(allData.bmpData.altitude, 2) + "\n" +
      "Aceleração em X: " + String(allData.imuData.accelX, 2) + "\n" +
      "Aceleração em Y: " + String(allData.imuData.accelY, 2) + "\n" +
      "Aceleração em Z: " + String(allData.imuData.accelZ, 2) + "\n" +
      "Quaternion W: " + String(allData.imuData.quaternion_w, 2) + "\n" +
      "Quaternion X: " + String(allData.imuData.quaternion_x, 2) + "\n" +
      "Quaternion Y: " + String(allData.imuData.quaternion_y, 2) + "\n" +
      "Quaternion Z: " + String(allData.imuData.quaternion_z, 2) + "\n" +
      "Paraquedas: " + String(allData.parachute) + "\n" +
      "Latitude: " + String(allData.gpsData.latitude, 6) + "\n" +
      "Longitude: " + String(allData.gpsData.longitude, 6) + "\n\n";

  return sdMessage;
}

void saveMessages() {
  updateFlightTelemetry();
  telemetry_message = telemetryMessage();
  sd_message = sdMessage();
  ++package_counter;
}
