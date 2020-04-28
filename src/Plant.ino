
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
  Particle.function("dashboardrequest", handleDashboardRequest);
  Particle.subscribe("hook-response/Weather", handleWeatherResponse,
                     MY_DEVICES);
  Particle.subscribe("TwitterMention", myHandler);
  Particle.subscribe("Google_Assistant", publishStatsWithAssistant);
  Particle.subscribe("GA_State", handleGAStateChange);
  // Particle.subscribe("GA_Open", openLidWithAssistant);
}

void loop() {

  if (Particle.connected()) {
    handleState();
    Alarm.delay(1);
    delay(3000);
  }
}

void handleState() {

  State state = slave.getState();
  Serial.print("State === ");

  if (state == WAITING && !sensorHandler.nightTime()) {
    return;
  }

  if (sensorHandler.nightTime()) {
    // night time
    slave.closeLid();
  } else {
    // day time

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

void handleGAStateChange(const char *event, String data) {

  if (data == "please close") {
    if (slave.closeLid()) {
      slave.setWaiting();
    }
  } else if (data == "please open") {
    slave.openLid();
  }
}

void setUpTimer() {
  Alarm.timerRepeat(60, publishTimedSensorData);
  Alarm.timerRepeat(3600, sendWeatherApiRequest);
}

int handleWeatherResponse(const char *event, String data) {
  if (data == "Light Rain" || data == "Heavy Rain") {
    if (slave.closeLid()) {
      slave.setWaiting();
    }
    hooks.sendSMS("Weather forecasting: Rain. Closing lid.");
    hooks.sendMail("Weather forecasting: Rain. Closing lid.");
  }
}

int handleDashboardRequest(String command) {
  Serial.println(command);
  if (command == "close") {
    if (slave.closeLid()) {
      slave.setWaiting();
    }
  } else if (command == "open") {
    slave.openLid();
  } else if (command == "safe") {
    slave.secureModeOn();
  } else if (command == "safe_off") {
    slave.secureModeOff();
  }
}

void sendWeatherApiRequest() { Particle.publish("Weather", "", PRIVATE); }
