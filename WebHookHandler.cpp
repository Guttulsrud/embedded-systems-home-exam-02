#include "WebHookHandler.h"

void WebHookHandler::setup() {
  Serial.println("hello!");
  Serial.println("hello!");
}

bool WebHookHandler::publishToWebHook(const char *webhook, const char *data) {
  return Particle.publish(webhook, data, PRIVATE);
}

void WebHookHandler::sendSMS(const char *message) {
  if (publishToWebHook("Twilio", message)) {
    Serial.println("Sending SMS!");
  } else {
    Serial.println("no can do SMS :(");
  }
}

void WebHookHandler::sendTweet(const char *message) {
  if (publishToWebHook("Twitter_Status", message)) {
    Serial.println("Tweeting!");
  } else {
    Serial.println("no can do tweet :(");
  }
}

void WebHookHandler::sendMail(const char *message) {
  if (publishToWebHook("Send_Email", message)) {
    Serial.println("Sending mail!");
  } else {
    Serial.println("no can do mail :(");
  }
}
