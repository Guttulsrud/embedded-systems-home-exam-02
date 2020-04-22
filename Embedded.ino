
 #include "Particle.h"
 #include "dht11.h"

 dht11 DHT11;


 void hookResponseHandler(const char *event, const char *data);


 int stepperUp = D7;
 int stepperDown = D6;
 int humidity = 0;
 int temperature = 0;
 int light = 0;
 String dhtData = String(0);
 int lightSensorPin = A0;

 void setup() {

   Serial.begin(9600);
   pinMode(stepperUp, OUTPUT);
   pinMode(stepperDown, OUTPUT);
   digitalWrite(stepperDown, LOW);
   digitalWrite(stepperUp, LOW);

  //  Particle.subscribe("hook-response/weather", hookResponseHandler, MY_DEVICES);
  //  takeReading();


 }


 void loop() {

     takeReading();
     delay(360000);
 }


 void hookResponseHandler(const char *event, const char *data) {
  //  String data2 = "{\"field3\":"+ String(data)+"}";
  //  Particle.publish("pressure", data, PRIVATE);
  //  Serial.println(data);
 }

 // function to take reading from DHT11
void takeReading() {
    // get initial reading to check status of board
    int chk = DHT11.read(2);

    // Serial.println("DEBUG Read sensor: ");
    // switch (chk)
    // {
    // case DHTLIB_OK:
    //     Particle.publish("DEBUG", "OK");
    // 	break;
    // case DHTLIB_ERROR_CHECKSUM:
    //     Particle.publish("DEBUG", "Checksum error");
    // 	break;
    // case DHTLIB_ERROR_TIMEOUT:
    // 	Particle.publish("DEBUG", "Time out error");
    // 	break;
    // default:
    // 	Particle.publish("DEBUG", "Unknown error");
    // 	break;
    // }
    //
    //

    // Light level measurement
    float light_measurement = analogRead(lightSensorPin);
    light = (int)(light_measurement/4096*100);


    temperature = DHT11.temperature;
    humidity = DHT11.humidity;
    dhtData = "{\"field1\":"+ String(temperature)+",\"field2\":"+String(humidity)+",\"field3\":"+String(light)+"}";


    // Particle.publish("temp", data, PRIVATE);
    Serial.println("sending!");
    publishToWebHook("temp", dhtData);
}

void publishToWebHook(const char *webhook, const char* data) {
  Particle.publish(webhook, data, PRIVATE);

}

void activateStepper(int direction) {
  digitalWrite(direction, HIGH);
  delay(100);
  digitalWrite(direction, LOW);
}


 void sendSMS(const char* message) {
   Particle.publish("Twilio", message, PRIVATE);
 }

 void postTwitterStatus(const char* message) {
   Particle.publish("Twitter_Status", message, 60, PRIVATE);
 }
