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
enum State {
  OPEN_BOX_SECURE,
  OPEN_BOX,
  CLOSED_BOX,
  TRIGGERED,
  WAITING
};
State state = OPEN_BOX_SECURE;

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
  if (state == OPEN_BOX_SECURE) {
      Serial.println("Secure mode on!");

    getDistance();
    if (distance < 6) {
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
  delay(1000);
}

void listenForI2cEvent() {
  if (listening) {

    switch (photonCommand) {
      case 0:
        closeLid();
        break;
      case 1:
        openLid();
        break;
      case 2:
        waterOn();
        break;
      case 3:
        enableSecureMode();
        break;
      case 4:
        disableSecureMode();
        break;
      case 5:
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
  for (int i = 180; i > 10; i--) {
    servo.write(i);
    delay(18);
  }
  delay(5000);
  waterOff();
}

void closeLid() {
  digitalWrite(7, HIGH);
  myStepper.step(-stepsPerRevolution * 10);
  digitalWrite(7, LOW);
  state = CLOSED_BOX;
}

void openLid() {
  digitalWrite(7, HIGH);
  myStepper.step(stepsPerRevolution * 10);
  delay(1000);
  digitalWrite(7, LOW);
  state = OPEN_BOX_SECURE;
}
void receiveEvent (int howMany) {
  photonCommand = Wire.read();
  listening = true;
}

void enableSecureMode() {
  state = OPEN_BOX_SECURE;
}

void disableSecureMode() {
  state = OPEN_BOX;
}

void requestEvent()  {
  Wire.write(state);
}
