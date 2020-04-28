#include "SlaveHandler.h"

void SlaveHandler::setup() { state = SECURE_OFF_OPEN_LID; }

State SlaveHandler::getState() {
  Wire.requestFrom(0x08, 1);
  state = (State)Wire.read();
  return state;
}

void SlaveHandler::closeLid() {
  if (state != CLOSED_LID || state != WAITING) {
    if (sendI2cCommand(CLOSE_LID)) {
      state = CLOSED_LID;
    }
  }
}

void SlaveHandler::openLid() {
  if (state != OPEN_LID) {
    if (sendI2cCommand(OPEN_LID)) {
      Serial.println("Open lid! ;D");

      state = SECURE_OFF_OPEN_LID;
    }
  }
}

void SlaveHandler::setWaiting() {

  if (sendI2cCommand(SET_WAITING)) {
    Serial.println("Idle...");
    state = WAITING;
  }
}

void SlaveHandler::waterOn() {

  if (sendI2cCommand(WATER_ON)) {
    // Watered!
  }
}

void SlaveHandler::waterOff() {

  if (sendI2cCommand(WATER_OFF)) {
    // Water off!
  }
}

bool SlaveHandler::sendI2cCommand(int command) {
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
