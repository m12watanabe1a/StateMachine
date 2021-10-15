#pragma once

#include <memory>
#include <stdio.h>
#include <typeinfo>
#include <cinttypes>
#include <cassert>
#include <vector>

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
      std::shared_ptr<StateMachine> sm,
      std::shared_ptr<const EventData> data) const = 0;
};

template <class SM, class Data, void (SM::*Func)(std::shared_ptr<const Data>)>
class StateAction : public StateBase
{
public:
  virtual void invokeStateAction(
      std::shared_ptr<StateMachine> sm,
      std::shared_ptr<const EventData> data) const
  {
    auto derived_sm = std::dynamic_pointer_cast<SM>(sm);
    auto derived_data = std::dynamic_pointer_cast<const Data>(data);

    assert(derived_data.get() != nullptr);
    (derived_sm.get()->*Func)(derived_data);
  }
};

class GuardBase
{
public:
  virtual bool invokeGuardCondition(
      std::shared_ptr<StateMachine> sm,
      std::shared_ptr<const EventData> data) const = 0;
};

template <class SM, class Data, bool (SM::*Func)(std::shared_ptr<const Data>)>
class GuardCondition : public GuardBase
{
public:
  virtual bool invokeGuardCondition(
      std::shared_ptr<StateMachine> sm,
      std::shared_ptr<const EventData> data) const
  {
    auto derived_sm = std::dynamic_pointer_cast<SM>(sm);
    auto derived_data = std::dynamic_pointer_cast<const Data>(data);
    assert(derived_data.get() != nullptr);

    return (derived_sm.get()->*Func)(derived_data);
  }
};

class EntryBase
{
public:
  virtual void invokeEntryAction(
      std::shared_ptr<StateMachine> sm,
      std::shared_ptr<const EventData> data) const = 0;
};

template <class SM, class Data, void (SM::*Func)(std::shared_ptr<const Data>)>
class EntryAction : public EntryBase
{
public:
  virtual void invokeEntryAction(
      std::shared_ptr<StateMachine> sm,
      std::shared_ptr<const EventData> data) const
  {
    auto derived_sm = std::dynamic_pointer_cast<SM>(sm);
    auto derived_data = std::dynamic_pointer_cast<const Data>(data);

    assert(derived_data.get() != nullptr);
    (derived_sm.get()->*Func)(derived_data);
  }
};

class ExitBase
{
public:
  virtual void invokeExitAction(
      std::shared_ptr<StateMachine> sm) const = 0;
};

template <class SM, void (SM::*Func)(void)>
class ExitAction : public ExitBase
{
public:
  virtual void invokeExitAction(
      std::shared_ptr<StateMachine> sm) const
  {
    auto derived_sm = std::dynamic_pointer_cast<SM>(sm);

    (derived_sm.get()->*Func)();
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

class StateMachine : public std::enable_shared_from_this<StateMachine>
{
public:
  enum
  {
    EVENT_IGNORED = 0xFE,
    CANNOT_HAPPEN
  };

  StateMachine(size_t max_states, uint8_t initial_state = 0);
  virtual ~StateMachine() {}
  uint8_t getCurrentState() { return this->current_state_; }
  size_t getMaxStates() { return this->max_states_; }

protected:
  void externalEvent(
      uint8_t new_state,
      std::shared_ptr<const EventData> data_ptr = nullptr);
  void internalEvent(
      uint8_t new_state,
      std::shared_ptr<const EventData> data_ptr = nullptr);

private:
  const size_t max_states_;
  uint8_t current_state_;
  uint8_t new_state_;

  bool event_generated_;

  std::shared_ptr<const EventData> event_data_ptr;
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
  void ST_##stateName(std::shared_ptr<const eventData>);  \
  StateAction<stateMachine, eventData, &stateMachine::ST_##stateName> stateName;

#define STATE_DEFINE(stateMachine, stateName, eventData) \
  void stateMachine::ST_##stateName(std::shared_ptr<const eventData> data)

#define GUARD_DECLARE(stateMachine, guardName, eventData) \
  bool GD_##guardName(std::shared_ptr<const eventData>);  \
  GuardCondition<stateMachine, eventData, &stateMachine::GD_##guardName> guardName;

#define GUARD_DEFINE(stateMachine, guardName, eventData) \
  bool stateMachine::GD_##guardName(std::shared_ptr<const eventData> data)

#define ENTRY_DECLARE(stateMachine, entryName, eventData) \
  void EN_##entryName(std::shared_ptr<const eventData>);  \
  EntryAction<stateMachine, eventData, &stateMachine::EN_##entryName> entryName;

#define ENTRY_DEFINE(stateMachine, entryName, eventData) \
  void stateMachine::EN_##entryName(std::shared_ptr<const eventData> data)

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
    externalEvent(state);                   \
    return;                                 \
  }

#define STATE_MAP_ENTRY(stateName) \
  stateName

#define STATE_MAP_ENTRY_EX(stateName) \
  {                                   \
    stateName, 0, 0, 0                \
  }

#define STATE_MAP_ENTRY_ALL_EX(stateName, guardName, entryName, exitName) \
  {                                                                       \
    stateName, guardName, entryName, exitName                             \
  }
