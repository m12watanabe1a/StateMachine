#include <assert.h>
#include "state_machine.hpp"

StateMachine::StateMachine(uint8_t maxStates) : _maxStates(maxStates),
                                                currentState(0),
                                                _eventGenerated(false),
                                                _pEventData(NULL)
{
}

// generates an external event. called once per external event
// to start the state machine executing
void StateMachine::ExternalEvent(uint8_t newState,
                                 std::shared_ptr<EventData> pData)
{
  // if we are supposed to ignore this event
  if (newState == EVENT_IGNORED)
  {
    // just delete the event data, if any
    pData.reset();
  }
  else
  {
    // TODO - capture software lock here for thread-safety if necessary

    // generate the event and execute the state engine
    InternalEvent(newState, pData);
    StateEngine();

    // TODO - release software lock here
  }
}

// generates an internal event. called from within a state
// function to transition to a new state
void StateMachine::InternalEvent(uint8_t newState,
                                 std::shared_ptr<EventData> pData)
{
  if (pData)
  {
    pData = std::make_shared<EventData>();
  }

  _pEventData = pData;
  _eventGenerated = true;
  currentState = newState;
}

// the state engine executes the state machine states
void StateMachine::StateEngine(void)
{
  // while events are being generated keep executing stateas
  while (_eventGenerated)
  {
    _eventGenerated = false; // event used up, reset flag

    assert(currentState < _maxStates);

    // get state map
    const StateStruct *pStateMap = GetStateMap();

    // execute the state passing in event data, if any
    (this->*pStateMap[currentState].pStateFunc)(_pEventData);

    // if event data was used, then delete it
    if (_pEventData)
    {
      _pEventData.reset();
    }
  }
}