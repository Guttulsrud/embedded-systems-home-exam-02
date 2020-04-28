
#include "SensorHandler.h"
#include "WebHookHandler.h"

#include "SlaveHandler.h"

#include "TimeAlarms/TimeAlarms.h"

#include <Wire.h>

SensorHandler sensorHandler;
WebHookHandler hooks;
SlaveHandler slave;

void setup() {

  Wire.begin();
  Serial.begin(9600);
  Time.zone(+2);
  setUpTimer();

  hooks.setup();
  slave.setup();
  Particle.function("led", handleDashboardRequest);
  Particle.subscribe("hook-response/Weather", handleWeatherResponse,
                     MY_DEVICES);
  Particle.subscribe("TwitterMention", myHandler);
  Particle.subscribe("Google_Assistant", publishStatsWithAssistant);
  Particle.subscribe("GA_Close", closeLidWithAssistant);
  // Particle.subscribe("GA_Open", openLidWithAssistant);
}

void loop() {

  delay(10000);
  if (Particle.connected()) {
    handleState();
    Alarm.delay(1);
    delay(3000);
  }
}

void handleState() {
  State state = slave.getState();
  if (state == WAITING) {
    return;
  }

  // night time
  if (sensorHandler.nightTime()) {
    slave.closeLid();
  } else {
    // DayTime
    if (state == CLOSED_LID) {
      slave.openLid();

    } else if (state == TRIGGERED) {
      Serial.println("triggered!");
      // hooks.sendSMS("Plant detected movement. Closing lid.");
      // hooks.sendMail("Plant detected movement. Closing lid.");
      slave.setWaiting();
    }
  }
}

void publishTimedSensorData() {
  if (hooks.publishToWebHook("temp", sensorHandler.getSensorDataJSON())) {
    Serial.println("Succsessfully published sensor data.");
  } else {
    Serial.println("Could not publish timed sensor data.");
  }
}

void myHandler(const char *event, const char *data) {
  Serial.println("New mention! :o");
  String message = String(
      "Hello! I'm " + String((int)(sensorHandler.getTemperature())) + "C, " +
      String((int)(sensorHandler.getHumidity())) + "% humid and I also get a " +
      String(sensorHandler.getLightLevel()) + "% light level! yay");
  hooks.sendTweet(message);
}

void publishStatsWithAssistant(const char *event, const char *data) {
  String message = String(
      "Hello! I am " + String((int)(sensorHandler.getTemperature())) + "C, " +
      String((int)(sensorHandler.getHumidity())) + "% humid and I also get a " +
      String(sensorHandler.getLightLevel()) + "% light level! yay");
  Serial.println("sending sms!");

  // postTwitterStatus(message);
  // sendSMS(message);
}

void closeLidWithAssistant(const char *event, const char *data) {

  slave.closeLid();
  Serial.println("lid closed");
}

void setUpTimer() {
  Alarm.timerRepeat(60, publishTimedSensorData);
  Alarm.timerRepeat(3600, sendWeatherApiRequest);
}

int handleWeatherResponse(const char *event, String data) {
  if (data == "Light Rain" || data == "Heavy Rain") {
    slave.closeLid();
    slave.setWaiting();
    hooks.sendSMS("Weather forecasting: Rain. Closing lid.");
    hooks.sendMail("Weather forecasting: Rain. Closing lid.");
  }
}

int handleDashboardRequest(String command) {
  Serial.println("hello???");

  if (command == "close") {
    slave.closeLid();
  } else if (command == "open") {
    slave.openLid();
  } else if (command == "safe") {

  } else if (command == "safe_off") {
  }
}

void sendWeatherApiRequest() { Particle.publish("Weather", "", PRIVATE); }
