#include<Wire.h>
#include <Stepper.h>
#include <Servo.h>
Servo servo;
byte photonCommand = 0;
bool listening = false;
const byte stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

const int trigPin = 4;
const int echoPin = 3;
long duration = 0;
int distance = 0;
bool movementTrigger = false;
enum State {
  SECURE_OPEN_LID,
  SECURE_OFF_OPEN_LID,
  CLOSED_LID,
  BUSY,
  TRIGGERED,
  WAITING
};
State state = SECURE_OFF_OPEN_LID;


void setup() {
  Serial.begin(9600);
  Wire.begin(8);
  myStepper.setSpeed(100);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  pinMode(7, OUTPUT);
  servo.attach(5);
  servo.write(10);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop(void) {
  listenForI2cEvent();
  Serial.print("State:");
  Serial.println(state);
  if (state == SECURE_OPEN_LID) {
    Serial.println("State: Secure");
    getDistance();
    if (distance < 6) {
      Serial.println("Movement detected!");
      closeLid();
      state = TRIGGERED;
    }
  }
  delay(3000);
}

void getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.println(distance);
  delay(1000);
}

void listenForI2cEvent() {
  if (listening) {

    switch (photonCommand) {
      case 0:
        Serial.println("Closing");
        closeLid();
        break;
      case 1:
        Serial.println("Opening:");
        openLid();
        break;
      case 2:
        waterOn();
        Serial.println("waterOn:");
        break;
      case 3:
        waterOff();
        Serial.println("waterOff:");
        break;
      case 4:
        enableSecureMode();
        break;
      case 5:
        disableSecureMode();
        break;
      case 6:
        delay(2000);
        Serial.println("Waiting run on Arduino");
        state = WAITING;
    }
    photonCommand = 0;
    listening = false;
  }
}


void waterOff() {
  for (int i = 10; i < 180; i++) {
    servo.write(i);
    delay(18);
  }
}

void waterOn() {
  Serial.println("waterOn()");

  for (int i = 180; i > 10; i--) {
    servo.write(i);
    delay(18);
  }
}


void closeLid() {
  state = BUSY;
  digitalWrite(7, HIGH);
  myStepper.step(-stepsPerRevolution * 10);
  digitalWrite(7, LOW);
  state = CLOSED_LID;
}

void openLid() {
  state = BUSY;
  digitalWrite(7, HIGH);
  myStepper.step(stepsPerRevolution * 10);
  delay(1000);
  digitalWrite(7, LOW);
  state = SECURE_OPEN_LID;
}
void receiveEvent (int howMany) {
  photonCommand = Wire.read();
  Serial.println(photonCommand);
  listening = true;
}

void enableSecureMode() {
  Serial.println("enableSecureMode()");
  state = SECURE_OPEN_LID;
}

void disableSecureMode() {
  Serial.println("disableSecureMode()");
  state = SECURE_OFF_OPEN_LID;
}


void requestEvent()  {
  Wire.write(state);
}
