#include "auxiliary.h"

float maxVelocity = 0;
float maxAcceleration = 0;
float maxAltitude = 0;

class DataFlight
{
private:
    int package;
    float altitude;
    float accelerationZ;
    float quaternionW;
    float quaternionX;
    float quaternionY;
    float quaternionZ;
    int parachute;
    bool skib1;
    bool skib2;
    float latitude;
    float longitude;
    float velocity;
    float velocityX;
    float velocityY;
    float velocityZ;
    float acceleration;
    float accelerationX;
    float accelerationY;
    // float maximumAltitude;
    // float maximumVelocity;
    // float maximumAcceleration;

public:
    // Construtor padrão
    DataFlight()
        : package(0), altitude(0.0), accelerationZ(0.0), quaternionW(1.0), 
          quaternionX(0.0), quaternionY(0.0), quaternionZ(0.0), parachute(0), 
          latitude(0.0), longitude(0.0), velocity(0.0), velocityX(0.0),
          velocityY(0.0), velocityZ(0.0), acceleration(0.0), accelerationX(0.0), 
          accelerationY(0.0) {}

    // Construtor com parâmetros
    DataFlight(String message)
    {
        package = extractNumber(message.substring(0, 5), 5);
        altitude = 0.001 * extractNumber(message.substring(5, 12), 7, true);
        accelerationZ = 0.01 * extractNumber(message.substring(12, 17), 5, true);
        quaternionW = 0.01 * extractNumber(message.substring(17, 21), 4, true);
        quaternionX = 0.01 * extractNumber(message.substring(21, 25), 4, true);
        quaternionY = 0.01 * extractNumber(message.substring(25, 29), 4, true);
        quaternionZ = 0.01 * extractNumber(message.substring(29, 33), 4, true);
        parachute = message.substring(33, 34).toInt(); 
        skib1 = (parachute > 0);
        skib2 = (parachute > 1);
        latitude = -0.0001 * extractNumber(message.substring(34, 40), 6);
        longitude = -0.0001 * extractNumber(message.substring(40, 47), 6);

        // Valores default que precisam mudar no futuro
        velocity = 1;
        velocityX = 5;
        velocityY = 10;
        velocityZ = 10;
        acceleration = 1;
        accelerationX = 1;
        accelerationY = 5;
    }

    // Getters
    int getPackage() const { return package; }
    float getAltitude() const { return altitude; }
    float getAccelerationZ() const { return accelerationZ; }
    float getQuaternionW() const { return quaternionW; }
    float getQuaternionX() const { return quaternionX; }
    float getQuaternionY() const { return quaternionY; }
    float getQuaternionZ() const { return quaternionZ; }
    int getParachute() const { return parachute; }
    bool getSkib1() const { return skib1; }
    bool getSkib2() const { return skib2; }
    float getLatitude() const { return latitude; }
    float getLongitude() const { return longitude; }
    float getVelocity() const { return velocity; }
    float getVelocityX() const { return velocityX; }
    float getVelocityY() const { return velocityY; }
    float getVelocityZ() const { return velocityZ; }
    float getAcceleration() const { return acceleration; }
    float getAccelerationX() const { return accelerationX; }
    float getAccelerationY() const { return accelerationY; }
    float getMaximumAltitude() { 
      if (altitude > maxAltitude) {
        maxAltitude = altitude;
      }
      return maxAltitude; 
    }
    float getMaximumAcceleration() { 
      if (acceleration > maxAcceleration){ 
        maxAcceleration = acceleration;
      }
      return maxAcceleration; 
    }
    float getMaximumVelocity() { 
      if (velocity > maxVelocity) {
        maxVelocity = velocity;
      }
      return maxVelocity; 
    }

    // Setters
    void setPackage(int pkg) { package = pkg; }
    void setAltitude(float alt) { altitude = alt; }
    void setAccelerationZ(float accZ) { accelerationZ = accZ; }
    void setQuaternionW(float quatW) { quaternionW = quatW; }
    void setQuaternionX(float quatX) { quaternionX = quatX; }
    void setQuaternionY(float quatY) { quaternionY = quatY; }
    void setQuaternionZ(float quatZ) { quaternionZ = quatZ; }
    void setParachute(int par) { parachute = par; }
    void setLatitude(float lat) { latitude = lat; }
    void setLongitude(float lon) { longitude = lon; }
    void setVelocity(float vel) { velocity = vel; }
    void setVelocityX(float velX) { velocityX = velX; }
    void setVelocityY(float velY) { velocityY = velY; }
    void setVelocityZ(float velZ) { velocityZ = velZ; }
    void setAcceleration(float acc) { acceleration = acc; }
    void setAccelerationX(float accX) { accelerationX = accX; }
    void setAccelerationY(float accY) { accelerationY = accY; }
    void setMaximumVelocity(float vel) { 
      if (vel > maxVelocity) {
        maxVelocity = vel;
      }
    }
    void setMaximumAcceleration(float acc) { 
      if (acc > maxAcceleration) {
        maxAcceleration = acc;
      }
    }
    void setMaximumAltitude(float alt) { 
      if (alt > maxAltitude) {
        maxAltitude = alt;
      } 
    }
};
