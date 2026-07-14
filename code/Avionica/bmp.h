#define BMP_ADDRESS 0x77
#define SEA_LOCAL_PRESSURE 1013.2

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

Adafruit_BMP3XX bmp;

float highestAltitude = 0;
float altitudeAtual = 0;

float initialAltitude = 0;
int altitudeReadQuantity = 5;

void updateBMP() {
  if (!bmp.performReading()) {
    Serial.println("Failed to read BMP");
    return;
  }
}

void getInitialAltitude() {
  int i = 0;
  for(i = 0; i < altitudeReadQuantity; i++) {
    updateBMP();
    initialAltitude = bmp.readAltitude(SEA_LOCAL_PRESSURE);
  }
}

void verifyBMP() {
  setupBMPFlag = bmp.begin_I2C(BMP_ADDRESS);
}

void setupBMP() {
  Serial.println("BMP390 conectado!");

  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_2X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_1); // sem filtro
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  delay(500);
  updateBMP();
}

void readBMP() {
  updateBMP();

  allData.bmpData.temperature = bmp.temperature;
  allData.bmpData.pressure = bmp.pressure;
  allData.bmpData.altitude = bmp.readAltitude(SEA_LOCAL_PRESSURE) - initialAltitude;

  altitudeAtual = allData.bmpData.altitude;

  // Atualiza a altura máxima
  if(altitudeAtual > highestAltitude) {
    highestAltitude = altitudeAtual;
  }
}

