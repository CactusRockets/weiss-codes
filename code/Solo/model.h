#pragma once
#include <math.h>
#include <stdint.h>

// Todos os valores e maximos vem da avionica. Nenhum valor de simulacao.
struct DataFlight {
  uint32_t sequence = 0;
  double maximumAltitude = NAN;
  double altitude = NAN;
  double maximumVelocity = NAN;
  double velocity = NAN;
  double maximumAcceleration = NAN;
  double acceleration = NAN;
  double velocityX = NAN;
  double velocityY = NAN;
  double velocityZ = NAN;
  double accelerationX = NAN;
  double accelerationY = NAN;
  double accelerationZ = NAN;
  double quaternion_w = NAN;
  double quaternion_x = NAN;
  double quaternion_y = NAN;
  double quaternion_z = NAN;
  double latitude = NAN;
  double longitude = NAN;
  bool skib1 = false;
  bool skib2 = false;
};
