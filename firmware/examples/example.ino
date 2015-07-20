// This #include statement was automatically added by the Spark IDE.
#include "tentacle-particle/tentacle-particle.h"

#define server "tentacle.octoblu.com"
#define port 80

// IPAddress server(192,168,100,9);
// #define port 8111

//Checkout http://tentacle.readme.io for setup and usage instructions
static const char uuid[]  = "INSERT YOUR UUID HERE";
static const char token[] = "INSERT YOUR TOKEN HERE";

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
