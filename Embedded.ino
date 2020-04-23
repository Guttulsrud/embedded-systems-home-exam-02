
#include "Adafruit_DHT_Particle.h"
#include "Particle.h"
#include <Wire.h>

#define DHTPIN 5

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void hookResponseHandler(const char *event, const char *data);

const int button = D7;

float humidity = 0;
float temperature = 0;
int light = 0;
String sensorDataJson = String(0);
int lightSensorPin = A0;
#include "TimeAlarms/TimeAlarms.h"

void setup() {
  Wire.begin();
  Serial.begin(9600);
  dht.begin();
  pinMode(button, INPUT_PULLUP);
  //  Particle.subscribe("hook-response/weather", hookResponseHandler,
  //  MY_DEVICES); takeReading();
  Time.zone(+2);
  Serial.begin(9600);
  setTimers();
}
const byte arduinoAdress = 0x08;
byte i2cError;
int i = 0;

void loop() {
  Alarm.delay(1);
  if (digitalRead(button) == LOW) {
    sendCommandToArduino(65);
    delay(2000);
  }
}

bool sendCommandToArduino(int command) {
  Wire.beginTransmission(arduinoAdress);
  Wire.write(command);
  i2cError = Wire.endTransmission();

  switch (i2cError) {
  case 0:
    Serial.println("Success!");
    return true;
  case 1:
    Serial.println("data too long to fit in transmit buffer");
    break;
  case 2:
    Serial.println("received NACK on transmit of address");
    break;
  case 3:
    Serial.println("received NACK on transmit of data");
    break;
  case 4:
    Serial.println("unknown error!");
    break;
  default:
    Serial.println("no work");
  }
  return false;
}

void setTimers() { Alarm.timerRepeat(1800, activateTimer); }

void activateTimer() {
  Serial.println("alarm eyo");

  if (setDhtDataFromSensor() && setLightDataFromSensor()) {
    sensorDataJson = "{\"field1\":" + String(temperature) +
                     ",\"field2\":" + String(humidity) +
                     ",\"field3\":" + String(light) + "}";

    publishToWebHook("temp", sensorDataJson);
  } else {
    Serial.println("Couldn't get sensor data!");
  }
}

bool setDhtDataFromSensor() {
  float h = dht.getHumidity();
  float t = dht.getTempCelcius();

  if (!isnan(h) || !isnan(t)) {
    temperature = t;
    humidity = h;
    return true;
  } else {
    return false;
  }
}

bool setLightDataFromSensor() {
  float light_measurement = analogRead(lightSensorPin);
  int lightLevel = (int)(light_measurement / 4096 * 100);

  if (lightLevel > 0 && lightLevel < 100) {
    light = lightLevel;
    return true;
  } else {
    return false;
  }
}

void publishToWebHook(const char *webhook, const char *data) {
  Particle.publish(webhook, data, PRIVATE);
}

void sendSMS(const char *message) {
  Particle.publish("Twilio", message, PRIVATE);
}

void postTwitterStatus(const char *message) {
  Particle.publish("Twitter_Status", message, 60, PRIVATE);
}

void hookResponseHandler(const char *event, const char *data) {
  //  String data2 = "{\"field3\":"+ String(data)+"}";
  //  Particle.publish("pressure", data, PRIVATE);
  //  Serial.println(data);
}
