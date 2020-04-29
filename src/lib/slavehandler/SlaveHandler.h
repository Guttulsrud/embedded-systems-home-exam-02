#ifndef SlaveHandler_h
#define SlaveHandler_h
#include "Particle.h"
#include "Wire.h"

enum State {
  SECURE_BOX_OPEN,
  BOX_OPEN,
  BOX_CLOSED,
  TRIGGERED,
  BOX_CLOSED_IDLE
};

enum SlaveCommands {
  CLOSE_BOX,
  OPEN_BOX,
  WATER,
  SECURE_MODE_ON,
  SECURE_MODE_OFF,
  SET_IDLE,
};

class SlaveHandler {
public:
  void setup();
  State getState();
  bool closeBox();
  void openBox();
  void secureModeOn();
  void secureModeOff();
  bool water();
  void setIdle();

private:
  State state;
  const byte arduinoAdress = 0x08;
  bool sendI2cCommand(int command);
};

#endif
