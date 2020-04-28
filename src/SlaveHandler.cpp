#include "SlaveHandler.h"

void SlaveHandler::setup() { state = SECURE_OFF_OPEN_LID; }

State SlaveHandler::getState() {
  Wire.requestFrom(0x08, 1);
  state = (State)Wire.read();
  return state;
}

bool SlaveHandler::closeLid() {

  if (state != CLOSED_LID) {
    if (sendI2cCommand(CLOSE_LID)) {
      Serial.println("closeLid()??");

      while (getState() != CLOSED_LID) {
      };
      state = CLOSED_LID;
      return true;
    } else
      return false;
  }
}

void SlaveHandler::openLid() {
  if (state == WAITING || state == CLOSED_LID) {
    if (sendI2cCommand(OPEN_LID)) {
      Serial.println("openLid()");
      state = SECURE_OFF_OPEN_LID;
    }
  }
}

void SlaveHandler::setWaiting() {

  if (sendI2cCommand(SET_WAITING)) {

    while (getState() != WAITING) {
    };
    state = WAITING;
  }
}

void SlaveHandler::secureModeOn() {
  if (sendI2cCommand(SECURE_MODE_ON)) {
    Serial.println("Secure mode on...");
    state = SECURE_OPEN_LID;
  }
}
void SlaveHandler::secureModeOff() {
  if (sendI2cCommand(SECURE_MODE_OFF)) {
    Serial.println("Secure mode off...");
    state = SECURE_OFF_OPEN_LID;
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
  byte i2cError = Wire.endTransmission();

  switch (i2cError) {
  case 0:
    return true;
  case 1:
    Serial.println("Data too long to fit in transmit buffer");
    break;
  case 2:
    Serial.println("Received NACK on transmit of address");
    break;
  case 3:
    Serial.println("Received NACK on transmit of data");
    break;
  case 4:
    Serial.println("Unknown error!");
    break;
  default:
    Serial.println("No work");
  }
  return false;
}
