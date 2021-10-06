#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H
#include <stdio.h>
#include <memory>
#include <cinttypes>

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
  StateMachine(uint8_t maxStates);
  virtual ~StateMachine() {}

protected:
  enum
  {
    EVENT_IGNORED = 0xFE,
    CANNOT_HAPPEN
  };
  uint8_t currentState;
  void ExternalEvent(uint8_t, std::shared_ptr<EventData> = nullptr);
  void InternalEvent(uint8_t, std::shared_ptr<EventData> = nullptr);
  virtual const StateStruct *GetStateMap() = 0;

private:
  const uint8_t _maxStates;
  bool _eventGenerated;
  std::shared_ptr<EventData> _pEventData;
  void StateEngine(void);
};

typedef void (StateMachine::*StateFunc)(std::shared_ptr<EventData>);
struct StateStruct
{
  StateFunc pStateFunc;
};

#endif //STATE_MACHINE_H