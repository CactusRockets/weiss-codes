#include "FastIMU.h"

#define RAD2DEG 57.2958
#define DEG2RAD 0.0174533
#define GRAVITY 9.80665

/* CONFIGURAÇÕES MPU */
#define IMU_ADDRESS 0x68

#define PERFORM_CALIBRATION true

MPU6050 IMU;

calData calib = { 0 };
AccelData IMUAccel;
GyroData IMUGyro;

float invSqrt(float x);

struct Quart {
  double q0, q1, q2, q3;

  Quart(double _q0, double _q1, double _q2, double _q3) {
    q0 = _q0;
    q1 = _q1;
    q2 = _q2;
    q3 = _q3;
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

double time_elapsed = 0;
bool is_initial_measurement = true;

Quart quat(1, 0, 0, 0);

void calibrate() {
  Serial.println("Calibrando...");
  Serial.println("Keep IMU level.");
  delay(5000);
  if (IMU.hasMagnetometer()) {
    IMU.calibrateMag(&calib);
  }
  IMU.calibrateAccelGyro(&calib);
  Serial.println("Calibration done!");
  Serial.println("Accel biases X/Y/Z: ");
  Serial.print(calib.accelBias[0]);
  Serial.print(", ");
  Serial.print(calib.accelBias[1]);
  Serial.print(", ");
  Serial.println(calib.accelBias[2]);
  Serial.println("Gyro biases X/Y/Z: ");
  Serial.print(calib.gyroBias[0]);
  Serial.print(", ");
  Serial.print(calib.gyroBias[1]);
  Serial.print(", ");
  Serial.println(calib.gyroBias[2]);
  if (IMU.hasMagnetometer()) {
    Serial.println("Mag biases X/Y/Z: ");
    Serial.print(calib.magBias[0]);
    Serial.print(", ");
    Serial.print(calib.magBias[1]);
    Serial.print(", ");
    Serial.println(calib.magBias[2]);
    Serial.println("Mag Scale X/Y/Z: ");
    Serial.print(calib.magScale[0]);
    Serial.print(", ");
    Serial.print(calib.magScale[1]);
    Serial.print(", ");
    Serial.println(calib.magScale[2]);
  }
  IMU.init(calib, IMU_ADDRESS);
  Serial.println("Calibrado!");
}

void verifyMPU() {
  int err = IMU.init(calib, IMU_ADDRESS);
  setupMPUFlag = (err == 0);
}

void setupMPU() {
  Serial.println("MPU6050 conectado!");

  delay(500);
  if(PERFORM_CALIBRATION) {
    calibrate();
  }
  time_elapsed = 0;
}

void readMPU() {
  IMU.update();
  IMU.getAccel(&IMUAccel);
  IMU.getGyro(&IMUGyro);

  // Por enquanto aceleracao no sistema inercial do sensor
  allData.imuData.accelX = IMUAccel.accelX;
  allData.imuData.accelY = IMUAccel.accelY;
  allData.imuData.accelZ = IMUAccel.accelZ;

  double accX = IMUAccel.accelX;
  double accY = IMUAccel.accelY;
  double accZ = IMUAccel.accelZ;

  double dt = (millis() - time_elapsed) / 1000.0;
  time_elapsed = millis() * 1.0;

  double gyroX = IMUGyro.gyroX * DEG2RAD;
  double gyroY = IMUGyro.gyroY * DEG2RAD;
  double gyroZ = IMUGyro.gyroZ * DEG2RAD;

  Quart Sw(0, gyroX, gyroY, gyroZ);
  Quart dQ = (quat*0.5)*Sw;

  quat = quat + (dQ*dt);
  double norm = invSqrt(quat.q0*quat.q0 + quat.q1*quat.q1 + quat.q2*quat.q2 + quat.q3*quat.q3);
  quat = quat * norm;

  allData.imuData.quaternion_w = quat.q0;
  allData.imuData.quaternion_x = quat.q1;
  allData.imuData.quaternion_y = quat.q2;
  allData.imuData.quaternion_z = quat.q3;
}

float invSqrt(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	y = y * (1.5f - (halfx * y * y));
	return y;
}