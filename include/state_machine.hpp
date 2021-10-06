#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include <stdio.h>
#include <typeinfo>
#include <cinttypes>
#include <cassert>

class EventData
{
public:
  virtual ~EventData() {}
  //XALLOCATOR
};

typedef EventData NoEventData;

class StateMachine;

class StateBase
{
public:
  virtual void InvokeStateAction(StateMachine *sm, const EventData *data) const = 0;
};

template <class SM, class Data, void (SM::*Func)(const Data *)>
class StateAction : public StateBase
{
public:
  virtual void InvokeStateAction(StateMachine *sm, const EventData *data) const
  {
    SM *derivedSM = static_cast<SM *>(sm);

    const Data *derivedData = dynamic_cast<const Data *>(data);
    assert(derivedData != NULL);

    (derivedSM->*Func)(derivedData);
  }
};

class GuardBase
{
public:
  virtual bool InvokeGuardCondition(StateMachine *sm, const EventData *data) const = 0;
};

template <class SM, class Data, bool (SM::*Func)(const Data *)>
class GuardCondition : public GuardBase
{
public:
  virtual bool InvokeGuardCondition(StateMachine *sm, const EventData *data) const
  {
    SM *derivedSM = static_cast<SM *>(sm);
    const Data *derivedData = dynamic_cast<const Data *>(data);
    assert(derivedData != NULL);

    // Call the guard function
    return (derivedSM->*Func)(derivedData);
  }
};

class EntryBase
{
public:
  virtual void InvokeEntryAction(StateMachine *sm, const EventData *data) const = 0;
};

template <class SM, class Data, void (SM::*Func)(const Data *)>
class EntryAction : public EntryBase
{
public:
  virtual void InvokeEntryAction(StateMachine *sm, const EventData *data) const
  {
    SM *derivedSM = static_cast<SM *>(sm);
    const Data *derivedData = dynamic_cast<const Data *>(data);
    assert(derivedData != NULL);

    (derivedSM->*Func)(derivedData);
  }
};

class ExitBase
{
public:
  virtual void InvokeExitAction(StateMachine *sm) const = 0;
};

template <class SM, void (SM::*Func)(void)>
class ExitAction : public ExitBase
{
public:
  virtual void InvokeExitAction(StateMachine *sm) const
  {
    SM *derivedSM = static_cast<SM *>(sm);

    (derivedSM->*Func)();
  }
};

struct StateMapRow
{
  const StateBase *const State;
};

struct StateMapRowEx
{
  const StateBase *const State;
  const GuardBase *const Guard;
  const EntryBase *const Entry;
  const ExitBase *const Exit;
};

class StateMachine
{
public:
  enum
  {
    EVENT_IGNORED = 0xFE,
    CANNOT_HAPPEN
  };

  ///	Constructor.
  ///	@param[in] maxStates - the maximum number of state machine states.
  StateMachine(uint8_t maxStates, uint8_t initialState = 0);

  virtual ~StateMachine() {}

  uint8_t GetCurrentState() { return m_currentState; }

  uint8_t GetMaxStates() { return MAX_STATES; }

protected:
  void ExternalEvent(uint8_t newState, const EventData *pData = NULL);

  void InternalEvent(uint8_t newState, const EventData *pData = NULL);

private:
  const uint8_t MAX_STATES;

  uint8_t m_currentState;

  uint8_t m_newState;

  bool m_eventGenerated;

  const EventData *m_pEventData;

  virtual const StateMapRow *GetStateMap() = 0;

  virtual const StateMapRowEx *GetStateMapEx() = 0;

  void SetCurrentState(uint8_t newState) { m_currentState = newState; }

  void StateEngine(void);
  void StateEngine(const StateMapRow *const pStateMap);
  void StateEngine(const StateMapRowEx *const pStateMapEx);
};

#define STATE_DECLARE(stateMachine, stateName, eventData) \
  void ST_##stateName(const eventData *);                 \
  StateAction<stateMachine, eventData, &stateMachine::ST_##stateName> stateName;

#define STATE_DEFINE(stateMachine, stateName, eventData) \
  void stateMachine::ST_##stateName(const eventData *data)

#define GUARD_DECLARE(stateMachine, guardName, eventData) \
  bool GD_##guardName(const eventData *);                 \
  GuardCondition<stateMachine, eventData, &stateMachine::GD_##guardName> guardName;

#define GUARD_DEFINE(stateMachine, guardName, eventData) \
  bool stateMachine::GD_##guardName(const eventData *data)

#define ENTRY_DECLARE(stateMachine, entryName, eventData) \
  void EN_##entryName(const eventData *);                 \
  EntryAction<stateMachine, eventData, &stateMachine::EN_##entryName> entryName;

#define ENTRY_DEFINE(stateMachine, entryName, eventData) \
  void stateMachine::EN_##entryName(const eventData *data)

#define EXIT_DECLARE(stateMachine, exitName) \
  void EX_##exitName(void);                  \
  ExitAction<stateMachine, &stateMachine::EX_##exitName> exitName;

#define EXIT_DEFINE(stateMachine, exitName) \
  void stateMachine::EX_##exitName(void)

#define BEGIN_TRANSITION_MAP \
  static const uint8_t TRANSITIONS[] = {

#define TRANSITION_MAP_ENTRY(entry) \
  entry,

#define END_TRANSITION_MAP(data)                       \
  }                                                    \
  ;                                                    \
  assert(GetCurrentState() < ST_MAX_STATES);           \
  ExternalEvent(TRANSITIONS[GetCurrentState()], data); \
  static_assert((sizeof(TRANSITIONS) / sizeof(uint8_t)) == ST_MAX_STATES);

#define PARENT_TRANSITION(state)            \
  if (GetCurrentState() >= ST_MAX_STATES && \
      GetCurrentState() < GetMaxStates())   \
  {                                         \
    ExternalEvent(state);                   \
    return;                                 \
  }

#define BEGIN_STATE_MAP                                         \
private:                                                        \
  virtual const StateMapRowEx *GetStateMapEx() { return NULL; } \
  virtual const StateMapRow *GetStateMap()                      \
  {                                                             \
    static const StateMapRow STATE_MAP[] = {

#define STATE_MAP_ENTRY(stateName) \
  stateName,

#define END_STATE_MAP                                                        \
  }                                                                          \
  ;                                                                          \
  static_assert((sizeof(STATE_MAP) / sizeof(StateMapRow)) == ST_MAX_STATES); \
  return &STATE_MAP[0];                                                      \
  }

#define BEGIN_STATE_MAP_EX                                  \
private:                                                    \
  virtual const StateMapRow *GetStateMap() { return NULL; } \
  virtual const StateMapRowEx *GetStateMapEx()              \
  {                                                         \
    static const StateMapRowEx STATE_MAP[] = {

#define STATE_MAP_ENTRY_EX(stateName) \
  {stateName, 0, 0, 0},

#define STATE_MAP_ENTRY_ALL_EX(stateName, guardName, entryName, exitName) \
  {stateName, guardName, entryName, exitName},

#define END_STATE_MAP_EX                                                       \
  }                                                                            \
  ;                                                                            \
  static_assert((sizeof(STATE_MAP) / sizeof(StateMapRowEx)) == ST_MAX_STATES); \
  return &STATE_MAP[0];                                                        \
  }

#endif // _STATE_MACHINE_H
