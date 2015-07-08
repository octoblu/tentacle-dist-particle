#include "meshblu-credentials.h"

MeshbluCredentials::MeshbluCredentials(const char* uuid, const char* token) {
  this->uuid = uuid;
  this->token = token;
}

const char* MeshbluCredentials::getUuid() const {
  return uuid;
}

const char* MeshbluCredentials::getToken() const {
  return token;
}
