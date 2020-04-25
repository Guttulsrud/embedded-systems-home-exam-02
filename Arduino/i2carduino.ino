#include<Wire.h>
#include <Stepper.h>
#include <Servo.h>
Servo servo;
byte val = 0;
bool listening = false;
const byte stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

// defines pins numbers
const int trigPin = 4;
const int echoPin = 3;
// defines variables
long duration;
int distance;



void setup() {
  Serial.begin(9600);
  Wire.begin(8);
  myStepper.setSpeed(100);
  Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);
  pinMode(7, OUTPUT);
  servo.attach(5);
  servo.write(10);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void loop(void) {
  distanceTest();
  listenForI2cEvent();
}

void distanceTest() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

}

void listenForI2cEvent() {
  if (listening) {
    switch (val) {
      case 1:
        turnStepperLeft();
        break;
      case 2:
        turnStepperRight();
        break;
      case 3:
        turnServoLeft();
        break;
      case 4:
        turnServoRight();
        break;
    }
    listening = false;
    val = 0;
  }
}


void turnServoRight() {
  for (int i = 10; i < 180; i++) {
    servo.write(i);
    delay(18);
  }
}

void turnServoLeft() {
  for (int i = 180; i > 10; i--) {
    servo.write(i);
    delay(18);
  }
}


void turnStepperLeft() {
  digitalWrite(7, HIGH);
  myStepper.step(-stepsPerRevolution * 100);
  digitalWrite(7, LOW);
}

void turnStepperRight() {
  digitalWrite(7, HIGH);
  myStepper.step(stepsPerRevolution * 100);
  digitalWrite(7, LOW);
}
void receiveEvent (int howMany) {
  val = Wire.read();
  Serial.println("Listening..");
  listening = true;


}
