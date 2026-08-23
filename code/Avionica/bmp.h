#include <Wire.h>
#include <Adafruit_BMP3XX.h>

#define SEA_LOCAL_PRESSURE 1013.2

#define I2C_SDA 21
#define I2C_SCL 22

// Limites de sanidade para a altitude relativa (metros).
// Nenhuma leitura legitima de um voo de 1 km cai fora disso.
#define ALT_MIN_VALIDA -200.0f
#define ALT_MAX_VALIDA 3000.0f

Adafruit_BMP3XX bmp;

extern bool setupBMPFlag; // Declaração externa para evitar conflito de redefinição

float highestAltitude = 0;
float altitudeAtual = 0;
float initialAltitude = 0;
int altitudeReadQuantity = 10;

// Contador de leituras rejeitadas, para diagnostico
unsigned int bmpRejectedReads = 0;

void updateBMP() {
  if (!setupBMPFlag) return;
  if (!bmp.performReading()) {
    Serial.println("Failed to read BMP");
  }
}

void verifyBMP() {
  Wire.begin(I2C_SDA, I2C_SCL);

  // Testa 0x76, se falhar tenta 0x77
  if (bmp.begin_I2C(0x76)) {
    Serial.println("BMP390 detectado no endereço 0x76!");
    setupBMPFlag = true;
  } else if (bmp.begin_I2C(0x77)) {
    Serial.println("BMP390 detectado no endereço 0x77!");
    setupBMPFlag = true;
  } else {
    Serial.println("ERRO CRÍTICO: BMP390 não encontrado!");
    setupBMPFlag = false;
  }
}

void getInitialAltitude() {
  if (!setupBMPFlag) return;

  float somaAltitude = 0;
  for(int i = 0; i < altitudeReadQuantity; i++) {
    updateBMP();
    somaAltitude += bmp.readAltitude(SEA_LOCAL_PRESSURE);
    delay(20); // Aguarda o ciclo de 50Hz do sensor para renovar a leitura
  }

  // Média real das leituras de solo
  initialAltitude = somaAltitude / (float)altitudeReadQuantity;

  Serial.print("Altitude inicial calibrada: ");
  Serial.println(initialAltitude);
}

void setupBMP() {
  if (!setupBMPFlag) return;

  // Ajuste anti-ruído sem gerar atraso perceptível de tempo real
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X); // Reduz ruído de pressão
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);   // Filtro leve de hardware
  bmp.setOutputDataRate(BMP3_ODR_25_HZ);

  delay(200);
  getInitialAltitude(); // Calibra o zero de altitude no solo
}

void readBMP() {
  if (!setupBMPFlag) return;

  updateBMP();

  float nova = bmp.readAltitude(SEA_LOCAL_PRESSURE) - initialAltitude;

  // Guarda de sanidade: uma leitura corrompida (I2C com ruido, pressao
  // zerada) contaminaria highestAltitude de forma permanente e poderia
  // acionar o skib ainda no solo.
  if (isnan(nova) || nova < ALT_MIN_VALIDA || nova > ALT_MAX_VALIDA) {
    bmpRejectedReads++;
    Serial.print("BMP: leitura descartada (");
    Serial.print(nova);
    Serial.print(" m) | total rejeitadas: ");
    Serial.println(bmpRejectedReads);
    return; // mantem os valores anteriores
  }

  allData.bmpData.temperature = bmp.temperature;
  allData.bmpData.pressure = bmp.pressure;
  allData.bmpData.altitude = nova;

  altitudeAtual = nova;

  if(altitudeAtual > highestAltitude) {
    highestAltitude = altitudeAtual;
  }
}