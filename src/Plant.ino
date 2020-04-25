
#include "../SensorHandler.h"
#include "Particle.h"
#include "TimeAlarms/TimeAlarms.h"
#include <Wire.h>
void hookResponseHandler(const char *event, const char *data);

const byte button = D7;
const byte arduinoAdress = 0x08;
byte i2cError;
SensorHandler sensorHandler;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
  Time.zone(+2);
  setUpTimer();
}

enum State { OPEN, CLOSED, CHANGING };

State state = OPEN;

void checkState() {
  if (state == CLOSED) {
    if (sensorHandler.getLightLevel() > 40) {
      // sendCommandToArduino(65);
    }
  } else if (state == OPEN) {
    if (sensorHandler.getLightLevel() > 40) {
      // sendCommandToArduino(55);
    }
  }
}

void loop() {
  if (Particle.connected()) {
    checkState();
  }
  publishContinualSensorData();
  delay(15000);
  Alarm.delay(1);
}

bool closeCase() {}

sendCommandToArduino(2);
}

void setUpTimer() { Alarm.timerRepeat(60, publishTimedSensorData); }

void publishContinualSensorData() {
  if (publishToWebHook("temp", sensorHandler.getMoistureAndLightJSON())) {
    Serial.println("Publish light & moisture");
  } else {
  }
}

void publishTimedSensorData() {
  if (publishToWebHook("temp", sensorHandler.getDTHSensorJSON())) {
    Serial.println("Succsessfully published timed sensor data.");
  } else {
    Serial.println("Could not publish timed sensor data.");
  }
}

bool publishToWebHook(const char *webhook, const char *data) {
  return Particle.publish(webhook, data, PRIVATE);
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
