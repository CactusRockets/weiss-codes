#include <BasicLinearAlgebra.h>
using namespace BLA;
#include <Kalman.h>

#define Nstate 3
#define Nobs 2

KALMAN<Nstate, Nobs> K;
BLA::Matrix<Nobs> obs;

#define sigma_baro 0.022088
#define sigma_accel 0.000225 * 50

extern PacketData allData;

float obtain_dt() {
  static unsigned long lastTime = 0;
  unsigned long now = millis();
  float dt = (lastTime == 0) ? 0.0f : (now - lastTime) / 1000.0f;
  lastTime = now;
  return dt;
}

void kalman_filter(){
       float dt = obtain_dt();

       K.F = {1.0, dt, 0.5*dt*dt,
              0.0, 1.0, dt,
              0.0, 0.0, 1.0};

       K.H = {1.0, 0.0, 0.0,
              0.0, 0.0, 1.0};

       float q = 0.5; // densidade espectral do ruído de jerk — parâmetro de tuning
       K.Q = {q*pow(dt,5)/20, q*pow(dt,4)/8, q*pow(dt,3)/6,
              q*pow(dt,4)/8,  q*pow(dt,3)/3, q*pow(dt,2)/2,
              q*pow(dt,3)/6,  q*pow(dt,2)/2, q*dt};

       K.R = {sigma_baro*sigma_baro, 0.0,
              0.0, sigma_accel*sigma_accel};

       obs(0) = allData.bmpData.altitude;
       obs(1) = allData.imuData.accelY;

       K.update(obs);

       allData.bmpData.altitude = K.x(0);
       allData.imuData.accelZ = K.x(2);
}