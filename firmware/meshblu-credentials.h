#ifndef MESHBLU_CREDENTIALS_H
#define MESHBLU_CREDENTIALS_H
#include <stddef.h>

struct MeshbluCredentials {

public:
  MeshbluCredentials(const char* uuid=NULL, const char* token=NULL);
  const char* getUuid() const;
  const char* getToken() const;

private:
  const char* uuid;
  const char* token;
};
#endif
