// This #include statement was automatically added by the Spark IDE.
#include "tentacle-particle/tentacle-particle.h"

#define server "tentacle.octoblu.com"
#define port 80

// IPAddress server(192,168,100,9);
// #define port 8111

static const char uuid[]  = "91f33395-847a-4d71-af25-fd3abe3371bc";
static const char token[] = "24fefe99413b16283f41c7cf8c82d376211392f7";

TCPClient conn;

TentacleArduino tentacle;
Pseudopod pseudopod(conn, conn, tentacle);

uint32_t lastPing = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("The Day of the Tentacle has begun!"));
  connectToServer();
}

void loop() {
  if (!isConnected()) {
    conn.stop();
    connectToServer();
  }

  readData();

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

  //send keepalive every 5 seconds.
  if( (millis() - lastPing) > 5000) {
    Serial.println(F("Pinging the server"));
    Serial.flush();
    lastPing = millis();
    return pseudopod.isConnected();

  }

  return true;
}

void readData() {
  while (conn.available()) {
    Serial.println(F("Received message"));
    Serial.flush();

    if(pseudopod.readMessage() == TentacleMessageTopic_action) {
      Serial.println(F("Got an action message"));
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
