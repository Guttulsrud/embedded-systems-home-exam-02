#ifndef SensorHandler_h
#define SensorHandler_h
#include "Adafruit_DHT_Particle.h"
#include "Particle.h"
#define DHTPIN 5
const int sensor_pin = A1;

#define DHTTYPE DHT11
DHT DHT(DHTPIN, DHTTYPE);

class SensorHandler {
public:
  SensorHandler() { DHT.begin(); }

  String getSensorDataJSON() {
    return "{\"field1\": " + String((int)getTemperature()) +
           ", \"field2\": " + String((int)getHumidity()) +
           ", \"field3\": " + String(getLightLevel()) +
           ", \"field4\": " + String((int)getMoisture()) + "}";
  }

  int getLightLevel() {
    float lightLevel = analogRead(A0);
    int newLightLevel = (int)(lightLevel / 4096 * 100);

    if (newLightLevel > 0 && newLightLevel < 100) {
      return newLightLevel;
    } else {
      return light;
    }
  }

  bool nightTime() { return getLightLevel() < 5; }

  float getMoisture() { return analogRead(sensor_pin) / 10; }

  float getHumidity() {
    float newHumidity = DHT.getHumidity();

    if (isnan(newHumidity)) {
      return humidity;
      // Throw error here.. Publish/Send mail
    }
    humidity = newHumidity;
    return newHumidity;
  }

  float getTemperature() {
    float newTemperature = DHT.getTempCelcius();

    if (isnan(newTemperature)) {
      return temperature;
      // Throw error here.. Publish/Send mail
    }
    temperature = newTemperature;
    return newTemperature;
  }

private:
  float humidity = 0;
  float temperature = 0;
  byte light = 0;
  float moisture;
};
#endif
