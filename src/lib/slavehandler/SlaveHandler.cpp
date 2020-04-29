#include "SlaveHandler.h"

void SlaveHandler::setup() { state = SECURE_BOX_OPEN; }

State SlaveHandler::getState() {
  Wire.requestFrom(arduinoAdress, 1);
  state = (State)Wire.read();
  return state;
}

bool SlaveHandler::closeBox() {

  if (state != BOX_CLOSED) {
    if (sendI2cCommand(CLOSE_BOX)) {
      while (getState() != BOX_CLOSED) {
      };
      state = BOX_CLOSED;
      return true;
    } else
      return false;
  }
}

void SlaveHandler::openBox() {
  if (state == BOX_CLOSED_IDLE || state == BOX_CLOSED) {
    if (sendI2cCommand(OPEN_BOX)) {
      state = SECURE_BOX_OPEN;
    }
  }
}

void SlaveHandler::setIdle() {
  if (sendI2cCommand(SET_IDLE)) {
    while (getState() != BOX_CLOSED_IDLE) {
    }
    state = BOX_CLOSED_IDLE;
  }
}

void SlaveHandler::secureModeOn() {
  if (sendI2cCommand(SECURE_MODE_ON)) {
    Serial.println("Secure mode on...");
    state = SECURE_BOX_OPEN;
  }
}
void SlaveHandler::secureModeOff() {
  if (sendI2cCommand(SECURE_MODE_OFF)) {
    Serial.println("Secure mode off...");
    state = BOX_OPEN;
  }
}

bool SlaveHandler::water() {
  if (sendI2cCommand(WATER)) {
    return true;
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
