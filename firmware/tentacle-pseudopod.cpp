#include "tentacle-pseudopod.h"
#include <stddef.h>

Pseudopod::Pseudopod(Stream &input, Print &output, Tentacle& tentacle) {
  pb_ostream_from_stream(output, pbOutput);
  pb_istream_from_stream(input, pbInput);

  this->tentacle = &tentacle;
  messagePinActions = new Action[tentacle.getNumPins()];
  resetPinActions();
}

bool Pseudopod::shouldBroadcastPins() {
  return broadcastPins;
}

int Pseudopod::getBroadcastInterval() {
  return broadcastInterval;
}

bool Pseudopod::isConfigured() {
  return configured;
}

void Pseudopod::resetPinActions() {
  for(int i = 0; i < tentacle->getNumPins(); i++) {
    messagePinActions[i] = Action_ignore;
  }
}

size_t Pseudopod::sendPins() {
  pbOutput.bytes_written = 0;

  currentMessage = {};
  currentMessage.topic = TentacleMessageTopic_action;
  currentMessage.has_topic = true;
  currentMessage.response = true;
  currentMessage.has_response = true;
  currentMessage.pins.funcs.encode = &Pseudopod::pinEncode;
  currentMessage.pins.arg = (void*)this;

  bool status = pb_encode_delimited(&pbOutput, TentacleMessage_fields, &currentMessage);

  return pbOutput.bytes_written;
}

size_t Pseudopod::sendPins(Action* actions) {
  resetPinActions();

  for(int i = 0; i < tentacle->getNumPins(); i++) {
    messagePinActions[i] = actions[i];
  }

  return sendPins();
}

size_t Pseudopod::sendConfiguredPins() {
  return sendPins(tentacle->getConfiguredPinActions());
}

size_t Pseudopod::authenticate(const char *uuid, const char *token) {
  pbOutput.bytes_written = 0;

  currentMessage = {};

  currentMessage.topic = TentacleMessageTopic_authentication;
  currentMessage.has_topic = true;
  currentMessage.authentication = {};
  currentMessage.has_authentication = true;

  strncpy(currentMessage.authentication.uuid, uuid, 36);
  currentMessage.authentication.has_uuid = true;

  strncpy(currentMessage.authentication.token, token, 40);

  currentMessage.authentication.has_token = true;

  bool status = pb_encode_delimited(&pbOutput, TentacleMessage_fields, &currentMessage);

  return pbOutput.bytes_written;
}

size_t Pseudopod::requestConfiguration() {
  pbOutput.bytes_written = 0;

  currentMessage = {};

  currentMessage.topic = TentacleMessageTopic_config;
  currentMessage.has_topic = true;

  bool status = pb_encode_delimited(&pbOutput, TentacleMessage_fields, &currentMessage);

  return pbOutput.bytes_written;
}

size_t Pseudopod::registerDevice() {
  return 0;
}

bool Pseudopod::isConnected() {
  pbOutput.bytes_written = 0;

  currentMessage = {};

  currentMessage.topic = TentacleMessageTopic_ping;
  currentMessage.has_topic = true;

  bool status = pb_encode_delimited(&pbOutput, TentacleMessage_fields, &currentMessage);

  return pbOutput.bytes_written != 0;
}

TentacleMessageTopic Pseudopod::readMessage() {
  resetPinActions();

  currentMessage = {};
  currentMessage.pins.funcs.decode = &Pseudopod::pinDecode;
  currentMessage.pins.arg = (void*) this;

  bool status = pb_decode_delimited(&pbInput, TentacleMessage_fields, &currentMessage);

  if (currentMessage.topic == TentacleMessageTopic_config) {
    configured = true;
    broadcastPins = currentMessage.broadcastPins;
    broadcastInterval = currentMessage.broadcastInterval;
  }

  return currentMessage.topic;
}

bool Pseudopod::pinEncode(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) {
  Pseudopod *pseudopod = (Pseudopod*) *arg;
  Action action;
  Pin pin;

  for(int i = 0; i < pseudopod->tentacle->getNumPins(); i++) {
    action = pseudopod->messagePinActions[i];
    if(action == Action_ignore) {
      continue;
    }

    pin = {};
    pin.has_action = true;
    pin.action = action;
    pin.has_number = true;
    pin.number = i;
    int value = pseudopod->tentacle->processPin(i);

    if(value != -1) {
      pin.has_value = true;
      pin.value = value;
    }

    if (!pb_encode_tag_for_field(stream, field)) {
      return false;
    }

    if(!pb_encode_submessage(stream, Pin_fields, &pin)) {
      return false;
    }
  }

  return true;
}


bool Pseudopod::pinDecode(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  Pseudopod *pseudopod = (Pseudopod*) *arg;
  Pin pin = {};

  if (!pb_decode(stream, Pin_fields, &pin)) {
    return false;
  }

  TentacleMessage& message = pseudopod->currentMessage;

  switch(message.topic) {
    case TentacleMessageTopic_config:
      pseudopod->tentacle->configurePin(pin.number, pin.action);
    break;

    case TentacleMessageTopic_action:
      pseudopod->messagePinActions[pin.number] = pin.action;
      pseudopod->tentacle->processPin(pin.number, pin.value);
    break;

  }

  return true;
}
