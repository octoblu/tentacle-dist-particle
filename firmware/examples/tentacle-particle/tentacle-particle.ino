// This #include statement was automatically added by the Spark IDE.
#include "tentacle-particle/tentacle-particle.h"

#define server "tentacle.octoblu.com"
#define port 80

static const char uuid[]  = "ff12c403-04c7-4e63-9073-2e3b1f8e4450";
static const char token[] = "28d2c24dfa0a5289799a345e683d570880a3bc41";

TCPClient conn;

TentacleArduino tentacle;
Pseudopod pseudopod(conn, conn, tentacle);

void setup() {
  Serial.begin(9600);
  Serial.println(F("The Day of the Tentacle has begun!"));
  connectToServer();
}

void loop() {
  if (!pseudopod.isConnected()) {
    conn.stop();
    connectToServer();
  }

  readData();

  if(!pseudopod.isConfigured()) {
    Serial.println(F("I'm not configured. Requesting configuration."));
    delay(300);
    pseudopod.requestConfiguration();
  }

  if(pseudopod.shouldBroadcastPins() ) {
    delay(pseudopod.getBroadcastInterval());
    size_t configSize = pseudopod.sendConfiguredPins();
    Serial.print(configSize);
    Serial.print(F(" bytes written while broadcasting pins"));
  }
}

bool isConnected() {
  if(!conn.connected()) {
    return false;
  }
  return pseudopod.isConnected();
}

void readData() {
  while (conn.available()) {
    Serial.println(F("Received message"));
    Serial.flush();

    if(pseudopod.readMessage() == TentacleMessageTopic_action) {
      pseudopod.sendPins();
    }
  }

}

void connectToServer() {
  int connectionAttempts = 0;
  Serial.println(F("Connecting to the server."));
  Serial.flush();

  while(!conn.connect(server, port)) {
    Serial.println(F("Can't connect to the server."));
    Serial.flush();
    conn.stop();
    connectionAttempts++;
  }

  size_t authSize = pseudopod.authenticate(uuid, token);
  Serial.print(authSize);
  Serial.println(F(" bytes written for authentication"));

}
