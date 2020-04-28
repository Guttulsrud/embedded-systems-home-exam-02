#ifndef WebHookHandler_h
#define WebHookHandler_h
#include "Particle.h"

class WebHookHandler {
public:
  void setup();
  bool publishToWebHook(const char *webhook, const char *data);

  void publishTimedSensorData();

  void sendSMS(const char *message);

  void sendTweet(const char *message);

  void sendMail(const char *message);
};

#endif
