#pragma once
#include <BasicLinearAlgebra.h>
#include "telemetry_math.h"
using namespace BLA;

#define Nstate 3
#define Nobs 2

// Mantem o modelo original de tres estados e duas observacoes.
// Kalman 1.1 depende de tipos removidos no BasicLinearAlgebra 5.x. Esta
// atualizacao usa diretamente as matrizes publicas de BLA, sem essa dependencia.
struct VerticalKalman {
  BLA::Matrix<3> x;
  BLA::Matrix<3, 3> F, P, Q;
  BLA::Matrix<2, 3> H;
  BLA::Matrix<2, 2> R;
  int status = 0;

  void update(const BLA::Matrix<2> &observation) {
    const BLA::Matrix<3> predicted = F * x;
    const BLA::Matrix<3, 3> covariance = F * P * (~F) + Q;
    BLA::Matrix<2, 2> innovation = H * covariance * (~H) + R;
    if (!BLA::Invert(innovation)) { status = 1; return; }
    const BLA::Matrix<3, 2> gain = covariance * (~H) * innovation;
    x = predicted + gain * (observation - H * predicted);
    BLA::Matrix<3, 3> identity;
    identity.Fill(0.0f);
    for (int i = 0; i < 3; ++i) identity(i, i) = 1.0f;
    const BLA::Matrix<3, 3> residual = identity - gain * H;
    // Forma de Joseph: reduz perda de precisao na covariancia.
    P = residual * covariance * (~residual) + gain * R * (~gain);
    status = 0;
    for (int i = 0; i < 3; ++i) {
      if (!isfinite(x(i))) status = 1;
      for (int j = 0; j < 3; ++j) if (!isfinite(P(i, j))) status = 1;
    }
  }
};
VerticalKalman K;
BLA::Matrix<Nobs> obs;

#define sigma_baro 0.022088
#define sigma_accel (0.000225 * 50 * FlightMath::GRAVITY_MS2)

extern PacketData allData;

bool kalmanSampleValid = false;
bool kalmanInitialized = false;
uint32_t lastKalmanTime = 0;
double verticalVelocity = NAN;
double verticalAcceleration = NAN;

void invalidateKalman() {
  kalmanSampleValid = false;
  kalmanInitialized = false;
  verticalVelocity = NAN;
  verticalAcceleration = NAN;
}

void kalman_filter(){
       kalmanSampleValid = false;
       FlightMath::Vector linear;
       if (!FlightMath::linearAcceleration(IMUAccel.accelX, IMUAccel.accelY, IMUAccel.accelZ,
             allData.imuData.quaternion_w, allData.imuData.quaternion_x,
             allData.imuData.quaternion_y, allData.imuData.quaternion_z, linear) ||
           !isfinite(allData.bmpData.altitude)) {
         invalidateKalman();
         return;
       }
       const uint32_t now = millis();
       if (!kalmanInitialized) {
         K.x = {allData.bmpData.altitude, 0.0f, static_cast<float>(linear.z)};
         K.P.Fill(0.0f);
         for (int i = 0; i < Nstate; ++i) K.P(i, i) = 1.0f;
         lastKalmanTime = now;
         kalmanInitialized = true;
         return; // Velocidade exige ao menos duas amostras validas.
       }
       const float dt = static_cast<uint32_t>(now - lastKalmanTime) / 1000.0f;
       if (dt <= 0) return;
       lastKalmanTime = now;

       K.F = {1.0f, dt, 0.5f*dt*dt,
              0.0f, 1.0f, dt,
              0.0f, 0.0f, 1.0f};

       K.H = {1.0, 0.0, 0.0,
              0.0, 0.0, 1.0};

       const float q = 0.5f; // densidade espectral do ruido de jerk
       const float dt2 = dt*dt, dt3 = dt2*dt, dt4 = dt3*dt, dt5 = dt4*dt;
       K.Q = {q*dt5/20, q*dt4/8, q*dt3/6,
              q*dt4/8,  q*dt3/3, q*dt2/2,
              q*dt3/6,  q*dt2/2, q*dt};

       K.R = {sigma_baro*sigma_baro, 0.0,
              0.0, sigma_accel*sigma_accel};

       obs(0) = allData.bmpData.altitude;
       // O estado e vertical: metros, m/s e m/s^2, com gravidade removida.
       obs(1) = linear.z;

       K.status = 0;
       K.update(obs);

       if (K.status != 0 || !isfinite(K.x(0)) || !isfinite(K.x(1)) || !isfinite(K.x(2))) {
         invalidateKalman();
         return;
       }
       allData.bmpData.altitude = K.x(0);
       verticalVelocity = K.x(1);
       verticalAcceleration = K.x(2);
       kalmanSampleValid = true;
}
