#pragma once

#include <stdio.h>
#include <typeinfo>
#include <cinttypes>
#include <cassert>

class EventData
{
public:
  virtual ~EventData() {}
};

using NoEventData = EventData;

class StateMachine;
class StateBase
{
public:
  virtual void invokeStateAction(
      StateMachine *sm,
      const EventData *data) const = 0;
};

template <class SM, class Data, void (SM::*Func)(const Data *)>
class StateAction : public StateBase
{
public:
  virtual void invokeStateAction(StateMachine *sm, const EventData *data) const
  {
    SM *derived_sm = static_cast<SM *>(sm);

    const Data *derived_data = dynamic_cast<const Data *>(data);
    assert(derived_data != nullptr);

    (derived_sm->*Func)(derived_data);
  }
};

class GuardBase
{
public:
  virtual bool invokeGuardCondition(
      StateMachine *sm,
      const EventData *data) const = 0;
};

template <class SM, class Data, bool (SM::*Func)(const Data *)>
class GuardCondition : public GuardBase
{
public:
  virtual bool invokeGuardCondition(StateMachine *sm, const EventData *data) const
  {
    SM *derived_sm = static_cast<SM *>(sm);
    const Data *derived_data = dynamic_cast<const Data *>(data);
    assert(derived_data != NULL);

    return (derived_sm->*Func)(derived_data);
  }
};

class EntryBase
{
public:
  virtual void invokeEntryAction(StateMachine *sm, const EventData *data) const = 0;
};

template <class SM, class Data, void (SM::*Func)(const Data *)>
class EntryAction : public EntryBase
{
public:
  virtual void invokeEntryAction(StateMachine *sm, const EventData *data) const
  {
    SM *derived_sm = static_cast<SM *>(sm);
    const Data *derived_data = dynamic_cast<const Data *>(data);
    assert(derived_data != nullptr);

    (derived_sm->*Func)(derived_data);
  }
};

class ExitBase
{
public:
  virtual void invokeExitAction(
      StateMachine *sm) const = 0;
};

template <class SM, void (SM::*Func)(void)>
class ExitAction : public ExitBase
{
public:
  virtual void invokeExitAction(StateMachine *sm) const
  {
    SM *derived_sm = static_cast<SM *>(sm);

    (derived_sm->*Func)();
  }
};

struct StateMapRow
{
  const StateBase *const state;
};

struct StateMapRowEx
{
  const StateBase *const state;
  const GuardBase *const guard;
  const EntryBase *const entry;
  const ExitBase *const exit;
};

class StateMachine
{
public:
  enum
  {
    EVENT_IGNORED = 0xFE,
    CANNOT_HAPPEN
  };

  StateMachine(uint8_t max_states, uint8_t initial_state = 0);
  virtual ~StateMachine() {}
  uint8_t getCurrentState() { return this->current_state_; }
  uint8_t getMaxStates() { return this->max_states_; }

protected:
  void externalEvent(
      uint8_t new_state,
      const EventData *data_ptr = nullptr);
  void internalEvent(
      uint8_t new_state,
      const EventData *data_ptr = nullptr);

private:
  const uint8_t max_states_;
  uint8_t current_state_;
  uint8_t new_state_;

  bool event_generated_;

  const EventData *event_data_ptr;
  virtual const StateMapRow *getStateMap() = 0;
  virtual const StateMapRowEx *getStateMapEx() = 0;

  void setCurrentState(uint8_t new_state)
  {
    this->current_state_ = new_state;
  }

  void stateEngine(void);
  void stateEngine(const StateMapRow *const state_map_ptr);
  void stateEngine(const StateMapRowEx *const state_map_ex_ptr);
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

#define TRANSITION_MAP_ENTRY(entry) \
  entry,

#define END_TRANSITION_MAP(data)                       \
  assert(getCurrentState() < ST_MAX_STATES);           \
  externalEvent(TRANSITIONS[getCurrentState()], data); \
  static_assert((sizeof(TRANSITIONS) / sizeof(uint8_t)) == ST_MAX_STATES, "STATE SIZE IS INVAILD");

#define PARENT_TRANSITION(state)            \
  if (getCurrentState() >= ST_MAX_STATES && \
      getCurrentState() < getMaxStates())   \
  {                                         \
    ExternalEvent(state);                   \
    return;                                 \
  }
