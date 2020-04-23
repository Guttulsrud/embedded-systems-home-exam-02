
#include "Adafruit_DHT_Particle.h"
#include "Particle.h"

#define DHTPIN 5

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void hookResponseHandler(const char *event, const char *data);

int stepperUp = D7;
int stepperDown = D6;
float humidity = 0;
float temperature = 0;
int light = 0;
String dhtData = String(0);
int lightSensorPin = A0;
#include "TimeAlarms/TimeAlarms.h"

void setup() {

  Serial.begin(9600);
  dht.begin();

  pinMode(stepperUp, OUTPUT);
  pinMode(stepperDown, OUTPUT);
  digitalWrite(stepperDown, LOW);
  digitalWrite(stepperUp, LOW);

  //  Particle.subscribe("hook-response/weather", hookResponseHandler,
  //  MY_DEVICES); takeReading();
  Time.zone(+1);
  Serial.begin(9600);

  delay(3000);

  for (int i = 0; i < 25; i++) {

    Alarm.alarmRepeat(i, 00, 0, EveningAlarm); // 5:45pm every day
  }
}

void loop() {

  Alarm.delay(1000);

  delay(2000);
  // takeReading();
  // delay(15000);
}

void EveningAlarm() { takeReading(); }

void hookResponseHandler(const char *event, const char *data) {
  //  String data2 = "{\"field3\":"+ String(data)+"}";
  //  Particle.publish("pressure", data, PRIVATE);
  //  Serial.println(data);
}

// function to take reading from DHT11
void takeReading() {
  float h = dht.getHumidity();
  float t = dht.getTempCelcius();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } else {
    temperature = t;
    humidity = h;
  }

  // todo: move out of this function.
  float light_measurement = analogRead(lightSensorPin);
  light = (int)(light_measurement / 4096 * 100);

  dhtData = "{\"field1\":" + String(temperature) +
            ",\"field2\":" + String(humidity) + ",\"field3\":" + String(light) +
            "}";

  publishToWebHook("temp", dhtData);
}

void publishToWebHook(const char *webhook, const char *data) {
  Particle.publish(webhook, data, PRIVATE);
}

void activateStepper(int direction) {
  digitalWrite(direction, HIGH);
  delay(100);
  digitalWrite(direction, LOW);
}

void sendSMS(const char *message) {
  Particle.publish("Twilio", message, PRIVATE);
}

void postTwitterStatus(const char *message) {
  Particle.publish("Twitter_Status", message, 60, PRIVATE);
}
