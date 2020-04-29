
#include "TimeAlarms/TimeAlarms.h"
#include "lib/sensorhandler/SensorHandler.h"
#include "lib/slavehandler/SlaveHandler.h"
#include "lib/webhookhandler/WebHookHandler.h"
#include <Wire.h>

SensorHandler sensorHandler;
WebHookHandler hooks;
SlaveHandler slave;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Time.zone(+2);
  setUpTimers();
  setUpHandlers();
  slave.setup();
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
  if (state == BOX_CLOSED_IDLE) {
    return;
  }

  if (sensorHandler.nightTime()) {
    slave.closeBox();
  } else {
    if (state == BOX_CLOSED) {
      slave.openBox();
    } else if (state == TRIGGERED) {
      hooks.sendSMS("Plant detected movement. Closing box.");
      hooks.sendMail("Plant detected movement. Closing box.");
      slave.setIdle();
    }
  }
}

void setUpTimers() {
  Alarm.timerRepeat(900, checkIfNeedsWater);
  Alarm.timerRepeat(190, publishTimedSensorData);
  Alarm.timerRepeat(3600, sendWeatherApiRequest);
  Alarm.timerRepeat(3600, publishTimedTweets);
}

void setUpHandlers() {
  Particle.function("dashboardrequest", handleDashboardRequest);
  Particle.subscribe("hook-response/Weather", handleWeatherResponse,
                     MY_DEVICES);
  Particle.subscribe("TwitterMention", handleTwitterMention);
  Particle.subscribe("Google_Assistant", handleGATweetRequest);
  Particle.subscribe("GA_State", handleGAStateChange);
}

void publishTimedSensorData() {
  if (hooks.publishToWebHook("temp", sensorHandler.getSensorDataJSON())) {
    Serial.println("Succsessfully published sensor data.");
  } else {
    Serial.println("Could not publish timed sensor data.");
  }
}

void checkIfNeedsWater() {
  if (sensorHandler.moistureBelowThreshold()) {
    hooks.sendTweet("I am very dry. Please give me water..");
    if (slave.water()) {
      hooks.sendTweet("I have now been watered, thanks!");
    };
  }
}

int handleWeatherResponse(const char *event, String data) {
  if (data == "Light Rain" || data == "Heavy Rain") {
    if (slave.closeBox()) {
      slave.setIdle();
    }
    hooks.sendSMS("Weather forecasting Rain. Closing box.");
    delay(2000);
    hooks.sendMail("Weather forecasting Rain. Closing box.");
  }
}

int handleDashboardRequest(String command) {
  Serial.println(command);
  if (command == "close") {
    if (slave.closeBox()) {
      slave.setIdle();
    }
  } else if (command == "open") {
    slave.openBox();
  } else if (command == "safe") {
    slave.secureModeOn();
  } else if (command == "safe_off") {
    slave.secureModeOff();
  }
}

void handleTwitterMention(const char *event, const char *data) {
  Serial.println("New mention! :o");
  String message = String(
      "Hello! I am " + String((int)(sensorHandler.getTemperature())) + "C, " +
      String((int)(sensorHandler.getHumidity())) + "% humid and I also get a " +
      String(sensorHandler.getLightLevel()) +
      "% light level!, aswell as a moisture level of " +
      String((int)sensorHandler.getMoisture()));
  hooks.sendTweet(message);
}

void handleGATweetRequest(const char *event, const char *data) {
  String message = String(
      "Hello! I am " + String((int)(sensorHandler.getTemperature())) + "C, " +
      String((int)(sensorHandler.getHumidity())) + "% humid and I also get a " +
      String(sensorHandler.getLightLevel()) +
      "% light level, aswell as a moisture level of " +
      String((int)sensorHandler.getMoisture()) + "!");

  hooks.sendMail(message);
  delay(2000);
  hooks.sendTweet(message);
  delay(2000);
  hooks.sendSMS(message);
}

void publishTimedTweets() {
  String message =
      String("Current status: temperature: " +
             String((int)(sensorHandler.getTemperature())) +
             "C, humidity: " + String((int)(sensorHandler.getHumidity())) +
             "%, light: " + String(sensorHandler.getLightLevel()) +
             "%, moisture level: " + String((int)sensorHandler.getMoisture()));
  hooks.sendTweet(message);
}

void handleGAStateChange(const char *event, String data) {
  if (data == "please close") {
    if (slave.closeBox()) {
      slave.setIdle();
    }
  } else if (data == "please open") {
    slave.openBox();
  }
}

void sendWeatherApiRequest() { Particle.publish("Weather", "", PRIVATE); }
