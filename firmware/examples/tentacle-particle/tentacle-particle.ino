#include "tentacle-particle.h"

#define conn Serial

TentacleArduino tentacle;
Pseudopod pseudopod(conn, conn, tentacle);

void setup() {
  Serial.begin(57600);
}

void loop() {
  readData();

  if(!pseudopod.isConfigured()) {
    delay(100);
    pseudopod.requestConfiguration();
  }

  if(pseudopod.shouldBroadcastPins() ) {
    delay(pseudopod.getBroadcastInterval());
    pseudopod.sendConfiguredPins();
  }
}

void readData() {
  while (conn.available()) {
    if(pseudopod.readMessage() == TentacleMessageTopic_action) {
      pseudopod.sendPins();
    }
  }

}
