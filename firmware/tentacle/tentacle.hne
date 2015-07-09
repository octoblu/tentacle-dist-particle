#ifndef tentacle_h
#define tentacle_h

#include "proto-buf.h"
#include "meshblu-credentials.h"


class Tentacle {
  public:
    Tentacle(size_t numPins);
    ~Tentacle();

    Tentacle& configurePin(int number, Action action);
    Tentacle& configurePins(Action* actions);

    Action* getConfiguredPinActions();

    int processPin(int number, int value);
    int processPin(int pin);


    int getNumPins() const;

    virtual const MeshbluCredentials& getCredentials() = 0;
    virtual Tentacle& setCredentials(const char* uuid, const char* token) = 0;

    virtual Tentacle& setMode(int number, Action action) = 0;
    virtual Tentacle& digitalWrite(int number, int value) = 0;
    virtual Tentacle& analogWrite(int number, int value) = 0;
    virtual bool digitalRead(int number) = 0;
    virtual int analogRead(int number) = 0;


  protected:
    int numPins;
    Action* configuredPinActions;
    void resetPinActions();

};

#endif
