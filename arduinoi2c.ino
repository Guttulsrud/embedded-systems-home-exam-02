#include<Wire.h>
#include <Stepper.h>

byte val = 0;
bool listening = false;
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  Serial.begin(9600);
  Wire.begin(8);
  myStepper.setSpeed(100);
  Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);
  pinMode(7, OUTPUT);
}

void loop(void) {
  if (listening) {
    if (val == 55) {
      Serial.println("motor up");
      motorUp();
    }
    if (val == 65) {
      Serial.println("motor down");
      motorDown();
    }
    listening = false;
    val = 0;
  }
}



void motorUp() {
  digitalWrite(7, HIGH);
  myStepper.step(-stepsPerRevolution * 100);
  digitalWrite(7, LOW);
}

void motorDown() {
  digitalWrite(7, HIGH);
  myStepper.step(stepsPerRevolution * 100);
  digitalWrite(7, LOW);
}
void receiveEvent (int howMany) {
  val = Wire.read();
  Serial.println("Listening..");
  listening = true;
}

void requestEvent() {
  /*
    int potvalue = analogRead(A0);
    byte SlaveSend = map(potvalue, 0, 1023, 0, 127);
    Wire.write(SlaveSend);*/
}
