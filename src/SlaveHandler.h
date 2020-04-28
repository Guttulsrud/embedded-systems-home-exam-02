#ifndef SlaveHandler_h
#define SlaveHandler_h
#include "Particle.h"
#include "Wire.h"

enum State {
  SECURE_OPEN_LID,
  SECURE_OFF_OPEN_LID,
  CLOSED_LID,
  BUSY,
  TRIGGERED,
  WAITING
};

enum SlaveCommands {
  CLOSE_LID,
  OPEN_LID,
  WATER_ON,
  WATER_OFF,
  SECURE_MODE_ON,
  SECURE_MODE_OFF,
  SET_WAITING,
};

class SlaveHandler {
public:
  void setup();
  State getState();
  bool closeLid();
  void openLid();
  void secureModeOn();
  void secureModeOff();
  void waterOn();
  void waterOff();
  void setWaiting();

  State state;
  const byte arduinoAdress = 0x08;
  bool sendI2cCommand(int command);

private:
};

#endif
