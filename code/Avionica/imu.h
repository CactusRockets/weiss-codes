#ifndef IMU_H
#define IMU_H

#include <Wire.h>
#include <Arduino.h>

// Referências para variáveis globais declaradas em Avionica.ino
extern bool setupMPUFlag;
// Assumindo que a estrutura PacketData e a variável 'allData' já foram definidas antes de incluir o imu.h

#define RAD2DEG 57.2958
#define DEG2RAD 0.0174533
#define GRAVITY 9.80665

/* CONFIGURAÇÕES MPU */
#define IMU_ADDRESS 0x68
#define PERFORM_CALIBRATION true

struct AccelData { float accelX, accelY, accelZ; };
struct GyroData { float gyroX, gyroY, gyroZ; };
struct calData {
  float accelBias[3];
  float gyroBias[3];
};

static bool imuSampleValid = false;
static AccelData IMUAccel;
static GyroData IMUGyro;
static calData calib = { 0 };

float invSqrt(float x);

struct Quart {
  double q0, q1, q2, q3;

  Quart(double _q0, double _q1, double _q2, double _q3) {
    q0 = _q0; q1 = _q1; q2 = _q2; q3 = _q3;
  }

  Quart operator +(const Quart& q) {
    return Quart(q0+q.q0, q1+q.q1, q2+q.q2, q3+q.q3);
  }

  Quart operator *(const Quart& q) {
    return Quart(
        q0*q.q0 - q1*q.q1 - q2*q.q2 - q3*q.q3,
        q0*q.q1 + q1*q.q0 + q2*q.q3 - q3*q.q2,
        q0*q.q2 - q1*q.q3 + q2*q.q0 + q3*q.q1,
        q0*q.q3 + q1*q.q2 - q2*q.q1 + q3*q.q0
    );
  }

  Quart operator *(double n) {
    return Quart(n*q0, n*q1, n*q2, n*q3);
  }
};

static double time_elapsed = 0;
static Quart quat(1, 0, 0, 0);

// Função para tirar o MPU6050 do modo Sleep via I2C direto
inline bool acordarMPU() {
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(0x6B); // Registrador PWR_MGMT_1
  Wire.write(0x00); // Acorda o chip
  return (Wire.endTransmission() == 0);
}

inline void calibrate() {
  Serial.println("Calibrando...");
  Serial.println("Mantenha a IMU nivelada e parada.");
  delay(2000);

  long sumAccX = 0, sumAccY = 0, sumAccZ = 0;
  long sumGyroX = 0, sumGyroY = 0, sumGyroZ = 0;
  const int samples = 500;
  int validSamples = 0;

  for (int i = 0; i < samples; i++) {
    Wire.beginTransmission(IMU_ADDRESS);
    Wire.write(0x3B);
    if (Wire.endTransmission(false) != 0 ||
        Wire.requestFrom((uint8_t)IMU_ADDRESS, (size_t)14) != 14) {
      setupMPUFlag = false;
      Serial.println("Calibracao abortada: leitura I2C incompleta.");
      return;
    }
    ++validSamples;

    sumAccX += (int16_t)(Wire.read() << 8 | Wire.read());
    sumAccY += (int16_t)(Wire.read() << 8 | Wire.read());
    sumAccZ += (int16_t)(Wire.read() << 8 | Wire.read());
    Wire.read(); Wire.read(); // Ignora a leitura da temperatura
    sumGyroX += (int16_t)(Wire.read() << 8 | Wire.read());
    sumGyroY += (int16_t)(Wire.read() << 8 | Wire.read());
    sumGyroZ += (int16_t)(Wire.read() << 8 | Wire.read());

    delay(2);
  }

  if (validSamples != samples) return;

  calib.accelBias[0] = (sumAccX / (float)samples) / 16384.0f;
  calib.accelBias[1] = (sumAccY / (float)samples) / 16384.0f;
  calib.accelBias[2] = ((sumAccZ / (float)samples) - 16384) / 16384.0f;

  calib.gyroBias[0] = (sumGyroX / (float)samples) / 131.0f;
  calib.gyroBias[1] = (sumGyroY / (float)samples) / 131.0f;
  calib.gyroBias[2] = (sumGyroZ / (float)samples) / 131.0f;

  Serial.println("Calibracao Concluida!");
}

inline void verifyMPU() {
  setupMPUFlag = acordarMPU();
}

inline void setupMPU() {
  if (acordarMPU()) {
    Serial.println("MPU6050 conectado e ativo!");
  } else {
    Serial.println("Erro ao conectar no MPU6050!");
    return;
  }

  delay(100);
  if (PERFORM_CALIBRATION) {
    calibrate();
  }
  time_elapsed = millis();
}

inline void readMPU() {
  imuSampleValid = false;
  Wire.beginTransmission(IMU_ADDRESS);
  Wire.write(0x3B); // Endereço inicial dos registradores de dados
  if (Wire.endTransmission(false) != 0 ||
      Wire.requestFrom((uint8_t)IMU_ADDRESS, (size_t)14) != 14) {
    setupMPUFlag = false;
    time_elapsed = millis();
    return;
  }

  int16_t rawAccX = (Wire.read() << 8) | Wire.read();
  int16_t rawAccY = (Wire.read() << 8) | Wire.read();
  int16_t rawAccZ = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read(); // Ignora temperatura
  int16_t rawGyrX = (Wire.read() << 8) | Wire.read();
  int16_t rawGyrY = (Wire.read() << 8) | Wire.read();
  int16_t rawGyrZ = (Wire.read() << 8) | Wire.read();

  IMUAccel.accelX = (rawAccX / 16384.0f) - calib.accelBias[0];
  IMUAccel.accelY = (rawAccY / 16384.0f) - calib.accelBias[1];
  IMUAccel.accelZ = (rawAccZ / 16384.0f) - calib.accelBias[2];

  IMUGyro.gyroX = (rawGyrX / 131.0f) - calib.gyroBias[0];
  IMUGyro.gyroY = (rawGyrY / 131.0f) - calib.gyroBias[1];
  IMUGyro.gyroZ = (rawGyrZ / 131.0f) - calib.gyroBias[2];

  allData.imuData.accelX = IMUAccel.accelX;
  allData.imuData.accelY = IMUAccel.accelY;
  allData.imuData.accelZ = IMUAccel.accelZ;

  double dt = (millis() - time_elapsed) / 1000.0;
  time_elapsed = millis();

  double gyroX = IMUGyro.gyroX * DEG2RAD;
  double gyroY = IMUGyro.gyroY * DEG2RAD;
  double gyroZ = IMUGyro.gyroZ * DEG2RAD;

  Quart Sw(0, gyroX, gyroY, gyroZ);
  Quart dQ = (quat * 0.5) * Sw;

  quat = quat + (dQ * dt);
  double norm = invSqrt(quat.q0*quat.q0 + quat.q1*quat.q1 + quat.q2*quat.q2 + quat.q3*quat.q3);
  quat = quat * norm;

  allData.imuData.quaternion_w = quat.q0;
  allData.imuData.quaternion_x = quat.q1;
  allData.imuData.quaternion_y = quat.q2;
  allData.imuData.quaternion_z = quat.q3;
  imuSampleValid = true;
}

inline float invSqrt(float x) {
  return (isfinite(x) && x > 0.0f) ? 1.0f / sqrtf(x) : NAN;
}

#endif