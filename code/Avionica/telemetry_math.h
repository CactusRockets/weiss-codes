#pragma once
#include <math.h>

namespace FlightMath {
constexpr double GRAVITY_MS2 = 9.80665;
struct Vector { double x = NAN, y = NAN, z = NAN; };

// A IMU existente fornece g e quaternion corpo -> referencia inicial.
// Calibracao nivelada: Z da referencia aponta para cima; X/Y nao sao norte/leste.
inline bool linearAcceleration(double ax, double ay, double az,
                               double w, double x, double y, double z, Vector &out) {
  const double norm = sqrt(w*w + x*x + y*y + z*z);
  if (!isfinite(ax) || !isfinite(ay) || !isfinite(az) || !isfinite(norm) || norm < 0.001)
    return false;
  w /= norm; x /= norm; y /= norm; z /= norm;
  out.x = ((1-2*(y*y+z*z))*ax + 2*(x*y-w*z)*ay + 2*(x*z+w*y)*az) * GRAVITY_MS2;
  out.y = (2*(x*y+w*z)*ax + (1-2*(x*x+z*z))*ay + 2*(y*z-w*x)*az) * GRAVITY_MS2;
  out.z = (2*(x*z-w*y)*ax + 2*(y*z+w*x)*ay + (1-2*(x*x+y*y))*az - 1) * GRAVITY_MS2;
  return isfinite(out.x) && isfinite(out.y) && isfinite(out.z);
}

inline Vector gpsVelocity(double speedMs, double courseDegrees) {
  Vector result;
  if (!isfinite(speedMs) || speedMs < 0 || !isfinite(courseDegrees) ||
      courseDegrees < 0 || courseDegrees >= 360) return result;
  const double radians = courseDegrees * 0.017453292519943295;
  result.x = speedMs * sin(radians); // Leste
  result.y = speedMs * cos(radians); // Norte
  return result;
}

inline double magnitude(double x, double y, double z) {
  if (!isfinite(x) || !isfinite(y) || !isfinite(z)) return NAN;
  return sqrt(x*x + y*y + z*z);
}

inline void updateMaximum(double value, double &maximum) {
  if (isfinite(value) && (!isfinite(maximum) || value > maximum)) maximum = value;
}
} // namespace FlightMath
