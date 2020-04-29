#ifndef SensorHandler_h
#define SensorHandler_h
#include "Adafruit_DHT_Particle.h"
#include "Particle.h"
#define DHTPIN 5
#define moisturePin A1
#define lightPin A0

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

  bool nightTime() { return getLightLevel() < 5; }

  bool moistureBelowThreshold() { return getMoisture() < 100; }

  float getMoisture() { return analogRead(moisturePin) / 10; }

  int getLightLevel() {
    float lightLevel = analogRead(lightPin);
    int newLightLevel = (int)(lightLevel / 4096 * 100);

    if (newLightLevel > 0 && newLightLevel < 100) {
      return newLightLevel;
    } else {
      return light;
    }
  }

  float getHumidity() {
    float newHumidity = DHT.getHumidity();

    if (isnan(newHumidity)) {
      return humidity;
    }
    humidity = newHumidity;
    return newHumidity;
  }

  float getTemperature() {
    float newTemperature = DHT.getTempCelcius();

    if (isnan(newTemperature)) {
      return temperature;
    }
    temperature = newTemperature;
    return newTemperature;
  }

private:
  float humidity = 0;
  float temperature = 0;
  int light = 0;
  float moisture;
};
#endif
