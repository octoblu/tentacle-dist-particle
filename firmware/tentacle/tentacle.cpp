#include "tentacle.h"

// Make library cross-compatiable
// with Arduino, GNU C++ for tests, and Spark.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(SPARK)
#include "application.h"
#endif

Tentacle::Tentacle(size_t numPins) {
  this->numPins = numPins;
  configuredPinActions = new Action[numPins];
  resetPinActions();
}

Action* Tentacle::getConfiguredPinActions() {
  return configuredPinActions;
}

void Tentacle::resetPinActions() {
  for(int i = 0; i < numPins; i++) {
    configuredPinActions[i] = Action_ignore;
  }
}

Tentacle::~Tentacle() {
  delete configuredPinActions;
}

Tentacle& Tentacle::configurePin(int number, Action action) {

  if( number < 0 || number >= numPins) {
    return *this;
  }

  configuredPinActions[number] = action;
  if(action == Action_ignore) {
      return *this;
  }


  setMode(number, action);

  return *this;
}

Tentacle& Tentacle::configurePins(Action* actions) {

  for(int i = 0; i < numPins; i++) {
    configurePin(i, actions[i]);
  }

  return *this;
}

int Tentacle::processPin(int number, int value) {
  if( number < 0 || number >= numPins) {
    return -1;
  }

  Action action = configuredPinActions[number];
  switch(action) {

    case Action_digitalWrite:
      digitalWrite(number, value);
      return value;
    break;

    case Action_analogWrite:
      analogWrite(number, value);
      return value;
    break;
  }

  return processPin(number);
}

int Tentacle::processPin(int number) {
  if( number < 0 || number >= numPins) {
    return -1;
  }

  Action action = configuredPinActions[number];
  switch(action) {
    case Action_digitalRead:
    case Action_digitalReadPullup:
      return digitalRead(number);
    break;

    case Action_analogRead:
    case Action_analogReadPullup:
      return analogRead(number);
    break;

    default:
    break;
  }

  return -1;
}

int Tentacle::getNumPins() const {
  return numPins;
}
