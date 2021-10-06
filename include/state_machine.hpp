#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H
#include <stdio.h>

class EventData
{
public:
  virtual ~EventData(){};
};

struct StateStruct;

// base class for state machines
class StateMachine
{
public:
  StateMachine(unsigned char maxStates);
  virtual ~StateMachine() {}

protected:
  enum
  {
    EVENT_IGNORED = 0xFE,
    CANNOT_HAPPEN
  };
  unsigned char currentState;
  void ExternalEvent(unsigned char, EventData * = NULL);
  void InternalEvent(unsigned char, EventData * = NULL);
  virtual const StateStruct *GetStateMap() = 0;

private:
  const unsigned char _maxStates;
  bool _eventGenerated;
  EventData *_pEventData;
  void StateEngine(void);
};

typedef void (StateMachine::*StateFunc)(EventData *);
struct StateStruct
{
  StateFunc pStateFunc;
};

#endif //STATE_MACHINE_H