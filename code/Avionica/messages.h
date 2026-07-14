
String fixNumberSize(int num, int width, bool enableSignal = false)
{
  int numPositive = (num >= 0 ? num : -num);
  String formattedString = String(numPositive);

  while (formattedString.length() < width)
  {
    formattedString = "0" + formattedString;
  }

  if (!enableSignal)
    return formattedString;

  if (num < 0)
  {
    formattedString = "-" + formattedString;
  }
  else
  {
    formattedString = "+" + formattedString;
  }

  return formattedString;
}

String telemetryMessage()
{
  String result =
      fixNumberSize(package_counter, 5) + 
      fixNumberSize((int)(allData.bmpData.altitude * 1000), 6, true) + 
      fixNumberSize((int)(allData.imuData.accelZ * 100), 4, true) + 
      fixNumberSize((int)(allData.imuData.quaternion_w * 100), 3, true) + 
      fixNumberSize((int)(allData.imuData.quaternion_x * 100), 3, true) + 
      fixNumberSize((int)(allData.imuData.quaternion_y * 100), 3, true) + 
      fixNumberSize((int)(allData.imuData.quaternion_z * 100), 3, true) + 
      String(allData.parachute) + 
      fixNumberSize((int)(allData.gpsData.latitude * 10000), 6) + 
      fixNumberSize((int)(allData.gpsData.longitude * 10000), 6);

  Serial.println(result);

  return result;
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
      "Altitude: " + String(allData.bmpData.altitude * 1000) + "\n" +
      "Aceleração em X: " + String(allData.imuData.accelX * 100) + "\n" +
      "Aceleração em Y: " + String(allData.imuData.accelY * 100) + "\n" +
      "Aceleração em Z: " + String(allData.imuData.accelZ * 100) + "\n" +
      "Quaternion W: " + String(allData.imuData.quaternion_w * 100) + "\n" +
      "Quaternion X: " + String(allData.imuData.quaternion_x * 100) + "\n" +
      "Quaternion Y: " + String(allData.imuData.quaternion_y * 100) + "\n" +
      "Quaternion Z: " + String(allData.imuData.quaternion_z * 100) + "\n" +
      "Paraquedas: " + String(allData.parachute) + "\n" +
      "Latitude: " + String(allData.gpsData.latitude * 1000) + "\n" +
      "Longitude: " + String(allData.gpsData.longitude * 1000) + "\n\n";

  return sdMessage;
}


void saveMessages()
{
  telemetry_message = telemetryMessage();
  sd_message = sdMessage();
  package_counter++;
}