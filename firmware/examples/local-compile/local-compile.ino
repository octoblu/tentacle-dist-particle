// This #include statement was automatically added by the Spark IDE.
#include "tentacle-particle.h"

#define server "tentacle.octoblu.com"
// IPAddress server(52, 24, 209, 206);
#define port 80


#define localServer "jupiter.octoblu.com"
#define localPort 8111

static const char uuid[]  = "ff12c403-04c7-4e63-9073-2e3b1f8e4450";
static const char token[] = "28d2c24dfa0a5289799a345e683d570880a3bc41";

TCPClient conn;

bool hasConnectedToServer = false;
bool hasAuthenticated = false;

TentacleArduino tentacle;
Pseudopod pseudopod(conn, conn, tentacle);

void setup() {
  Serial.begin(9600);
  Serial.println(F("The Day of the Tentacle has begun!"));
  setupDebug();
}

void setupDebug() {
  Spark.function("wifiInfo", wifiInfoCmd);
  Spark.function("connect", connectToServerCmd);
  Spark.function("connectlocal", connectToLocalServerCmd);
  Spark.function("disconnect", disconnectFromServerCmd);
  Spark.function("authenticate", authenticateCmd);
  Spark.function("config", requestConfigCmd);
  Spark.function("sendPins", sendPinsCmd);
  Spark.function("dfu", dfuCmd);
}

int wifiInfoCmd(String cmd) {
  wifiInfo();
  return 0;
}

int connectToServerCmd(String cmd) {
  connectToServer();
  return 0;
}

int connectToLocalServerCmd(String cmd) {
  connectToLocalServer();
  return 0;
}

int disconnectFromServerCmd(String cmd) {
  disconnectFromServer();
  return 0;
}

int authenticateCmd(String cmd) {
  return authenticate();
}

int requestConfigCmd(String cmd) {
  return pseudopod.requestConfiguration();
}

int sendPinsCmd(String cmd) {
  return pseudopod.sendConfiguredPins();
}


int dfuCmd(String cmd) {
  System.dfu();
  return 0;
}

void loop() {
  Serial.println(F("In loop."));

  if(!hasConnectedToServer) {
    Serial.println(F("I haven't been told to connect yet. Waiting."));
    delay(1000);
    Serial.flush();
    return;
  }

  if(!hasAuthenticated) {
    Serial.println(F("I haven't been told to authenticate yet. Waiting."));
    delay(1000);
    Serial.flush();
    return;
  }

  if (!isConnected()) {
    Serial.println(F("I wasn't connected to the server!"));
    conn.stop();
    connectToServer();
  }

  Serial.flush();
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

  Spark.process();
}

bool isConnected() {
  if(!conn.connected()) {
    return false;
  }
  return pseudopod.isConnected();
}

void readData() {

  while (conn.available()) {
    Serial.println(F("!!!RECEIVED MESSAGE!!!!"));
    Serial.flush();

    if(pseudopod.readMessage() == TentacleMessageTopic_action) {
      pseudopod.sendPins();
    }
  }
}

void connectToServer() {
  hasAuthenticated = false;

  Serial.println(F("Connecting to the server."));
  Serial.flush();

  while(!conn.connect(server, port)) {
    Serial.println(F("Can't connect to the server."));
    Serial.flush();
    conn.stop();
    delay(500);
  }

  hasConnectedToServer = true;
}

void connectToLocalServer() {
  hasAuthenticated = false;
  Serial.println(F("Connecting to local server."));
  Serial.flush();

  while(!conn.connect(localServer, localPort)) {
    Serial.println(F("Can't connect to local server."));
    Serial.flush();
    conn.stop();
    delay(500);
  }

  hasConnectedToServer = true;
}

void disconnectFromServer() {
  Serial.println(F("Disconnecting from server."));
  Serial.flush();
  conn.stop();

  hasConnectedToServer = false;
  hasAuthenticated = false;
}

int authenticate() {
  size_t authSize = pseudopod.authenticate(uuid, token);
  Serial.print(authSize);
  Serial.println(F(" bytes written for authentication"));
  Serial.flush();

  hasAuthenticated = true;

  return authSize;
}

void wifiInfo() {
  Serial.println(F("\n\nDebugging PHOTON"));

  Serial.print(F("\nWiFi Ready:\t"));
  Serial.println(WiFi.ready());

  if (WiFi.ready()) {
      Serial.println(F("WiFI:\t\t\t ready"));
  } else {
    Serial.println(F("WiFI:\t\t\t NOT ready"));
  }

  if (Spark.connected()) {
      Serial.println(F("Spark Cloud:\t\t\t connected"));
  } else {
    Serial.println(F("Spark Cloud:\t\t\t NOT Connected"));
  }

  Serial.println(F("\nPinging google"));
  Serial.println(WiFi.ping(IPAddress(74,125,239,34)));

  Serial.println(F("Pinging Tentacle"));
  Serial.println(WiFi.ping(IPAddress(54,186,61,91)));

  Serial.println(WiFi.localIP());
  Serial.println(WiFi.subnetMask());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.SSID());

}
