int extractNumber(String numberText, int width, bool considerSignal = false)
{
    int slicePosition = considerSignal ? 1 : 0;
    while (numberText[slicePosition] == '0' && slicePosition < width)
    {
        slicePosition++;
    }

    String slicedNumber = numberText.substring(slicePosition, width);

    int realNumber = slicedNumber.toInt();

    if (!considerSignal)
        return realNumber;
    else
    {
        if (numberText[0] == '+')
        {
            return realNumber;
        }
        if (numberText[0] == '-')
        {
            return -1 * realNumber;
        }
    }
}

void debugMessage(String message)
{
    int package = extractNumber(message.substring(0, 5), 5);
    float altitude = 0.001 * extractNumber(message.substring(5, 12), 7, true);
    float accelerationZ = 0.01 * extractNumber(message.substring(12, 17), 5, true);
    float quaternionW = 0.01 * extractNumber(message.substring(17, 21), 4, true);
    float quaternionX = 0.01 * extractNumber(message.substring(21, 25), 4, true);
    float quaternionY = 0.01 * extractNumber(message.substring(25, 29), 4, true);
    float quaternionZ = 0.01 * extractNumber(message.substring(29, 33), 4, true);
    int parachute = message.substring(33, 34).toInt();
    bool skib1 = (parachute > 0) || skib1;
    bool skib2 = (parachute > 1) || skib2;
    float latitude = -0.0001 * extractNumber(message.substring(34, 40), 6);
    float longitude = -0.0001 * extractNumber(message.substring(41, 47), 6);
    float velocity = 1;
    float velocityX = 5;
    float velocityY = 10;
    float velocityZ = 10;
    float maximumVelocity = 1;
    float maximumAcceleration = 1;
    float acceleration = 1;
    float accelerationX = 1;
    float accelerationY = 5;
    float maximumAltitude = 10;

    Serial.println("-------------------------------------");
    Serial.println("Pacote: " + String(package));
    Serial.println("Altitude: " + String(altitude, 3));
    Serial.println("Aceleração em Z: " + String(accelerationZ, 2));
    Serial.println("Quaternion W: " + String(quaternionW, 2));
    Serial.println("Quaternion X: " + String(quaternionX, 2));
    Serial.println("Quaternion Y: " + String(quaternionY, 2));
    Serial.println("Quaternion Z: " + String(quaternionZ, 2));
    Serial.println("Parachute Value: " + String(parachute, 1));
    Serial.println("Drogo Ativado? : " + String(skib1 ? "Sim" : "Não"));
    Serial.println("Paraquedas Principal Ativado? : " + String(skib2 ? "Sim" : "Não"));
    Serial.println("Latitude: " + String(latitude, 6));
    Serial.println("Longitude: " + String(longitude, 6));
    Serial.println("");
}

void debugMessageString(String message)
{
    // Extração de substrings como String
    String package = message.substring(0, 5);
    String altitude = message.substring(5, 12);
    String accelerationZ = message.substring(12, 17);
    String quaternionW = message.substring(17, 21);
    String quaternionX = message.substring(21, 25);
    String quaternionY = message.substring(25, 29);
    String quaternionZ = message.substring(29, 33);
    String parachute = message.substring(33, 34);
    String latitude = message.substring(34, 40);
    String longitude = message.substring(41, 47);

    // Impressão das substrings para debug
    Serial.println("-------------------------------------");
    Serial.println("Pacote: " + package);
    Serial.println("Altitude: " + altitude);
    Serial.println("Aceleração em Z: " + accelerationZ);
    Serial.println("Quaternion W: " + quaternionW);
    Serial.println("Quaternion X: " + quaternionX);
    Serial.println("Quaternion Y: " + quaternionY);
    Serial.println("Quaternion Z: " + quaternionZ);
    Serial.println("Parachute Value: " + parachute);
    Serial.println("Latitude: " + latitude);
    Serial.println("Longitude: " + longitude);
    Serial.println("");
}