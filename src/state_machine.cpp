#include "state_machine.hpp"
#include <cinttypes>
#include <cassert>

StateMachine::StateMachine(
    uint8_t max_states,
    uint8_t initial_state)
    : max_states_(max_states),
      current_state_(initial_state),
      new_state_(false),
      event_generated_(false),
      event_data_ptr(nullptr)
{
  assert(max_states_ < EVENT_IGNORED);
}

void StateMachine::externalEvent(
    uint8_t new_state,
    const EventData *data_ptr)
{
  // If we are supposed to ignore this event
  if (new_state == EVENT_IGNORED)
  {
#ifndef EXTERNAL_EVENT_NO_HEAP_DATA
    // Just delete the event data, if any
    if (data_ptr != nullptr)
    {
      delete data_ptr;
    }
#endif
  }
  else
  {
    // TODO - capture software lock here for thread-safety if necessary

#ifdef EXTERNAL_EVENT_NO_HEAP_DATA
    EventData data;
    if (data_ptr == nullptr)
    {
      data_ptr = &data;
    }
#endif
    // Generate the event
    this->internalEvent(new_state, data_ptr);

    // Execute the state engine. This function call will only return
    // when all state machine events are processed.
    this->stateEngine();

    // TODO - release software lock here
  }
}

void StateMachine::internalEvent(
    uint8_t new_state,
    const EventData *data_ptr)
{
  if (data_ptr == nullptr)
  {
    data_ptr = new NoEventData();
  }

  this->event_data_ptr = data_ptr;
  event_generated_ = true;
  this->new_state_ = new_state;
}

void StateMachine::stateEngine(void)
{
  const StateMapRow *state_map_ptr = this->getStateMap();
  if (state_map_ptr != nullptr)
  {
    this->stateEngine(state_map_ptr);
  }
  else
  {
    const StateMapRowEx *state_map_ex_ptr = this->getStateMapEx();
    if (state_map_ex_ptr != nullptr)
    {
      this->stateEngine(state_map_ex_ptr);
    }
    else
    {
      assert(false);
    }
  }
}

void StateMachine::stateEngine(
    const StateMapRow *const state_map_ptr)
{
#if EXTERNAL_EVENT_NO_HEAP_DATA
  bool external_event = true;
#endif
  const EventData *data_ptr_tmp = nullptr;

  // While events are being generated keep executing states
  while (this->event_generated_)
  {
    // Error check that the new state is valid before proceeding
    assert(this->new_state_ < this->max_states_);

    // Get the pointer from the state map
    const StateBase *state = state_map_ptr[this->new_state_].state;

    // Copy of event data pointer
    data_ptr_tmp = this->event_data_ptr;

    // Event data used up, reset the pointer
    this->event_data_ptr = nullptr;

    // Event used up, reset the flag
    this->event_generated_ = false;

    // Switch to the new current state
    this->setCurrentState(this->new_state_);

    // Execute the state action passing in event data
    assert(state != NULL);
    state->invokeStateAction(this, data_ptr_tmp);

    // If event data was used, then delete it
#if EXTERNAL_EVENT_NO_HEAP_DATA
    if (data_ptr_tmp)
    {
      if (!external_event)
      {
        delete data_ptr_tmp;
      }
      data_ptr_tmp = nullptr;
    }
    external_event = false;
#else
    if (data_ptr_tmp)
    {
      delete data_ptr_tmp;
      data_ptr_tmp = nullptr;
    }
#endif
  }
}

void StateMachine::stateEngine(
    const StateMapRowEx *const state_map_ex_ptr)
{
#if EXTERNAL_EVENT_NO_HEAP_DATA
  bool external_event = true;
#endif
  const EventData *data_ptr_tmp = NULL;

  // While events are being generated keep executing states
  while (this->event_generated_)
  {
    // Error check that the new state is valid before proceeding
    assert(this->new_state_ < this->max_states_);

    // Get the pointers from the state map
    const StateBase *state = state_map_ex_ptr[this->new_state_].state;
    const GuardBase *guard = state_map_ex_ptr[this->new_state_].guard;
    const EntryBase *entry = state_map_ex_ptr[this->new_state_].entry;
    const ExitBase *exit = state_map_ex_ptr[this->current_state_].exit;

    // Copy of event data pointer
    data_ptr_tmp = this->event_data_ptr;

    // Event data used up, reset the pointer
    this->event_data_ptr = nullptr;

    // Event used up, reset the flag
    this->event_generated_ = false;

    // Execute the guard condition
    bool guard_result = true;
    if (guard != nullptr)
    {
      guard_result = guard->invokeGuardCondition(this, data_ptr_tmp);
    }

    // If the guard condition succeeds
    if (guard_result == true)
    {
      // Transitioning to a new state?
      if (this->new_state_ != this->current_state_)
      {
        // Execute the state exit action on current state before switching to new state
        if (exit != nullptr)
        {
          exit->invokeExitAction(this);
        }

        // Execute the state entry action on the new state
        if (entry != nullptr)
        {
          entry->invokeEntryAction(this, data_ptr_tmp);
        }

        // Ensure exit/entry actions didn't call InternalEvent by accident
        assert(this->event_generated_ == false);
      }

      // Switch to the new current state
      this->setCurrentState(this->new_state_);

      // Execute the state action passing in event data
      assert(state != nullptr);
      state->invokeStateAction(this, data_ptr_tmp);
    }

    // If event data was used, then delete it
#if EXTERNAL_EVENT_NO_HEAP_DATA
    if (data_ptr_tmp)
    {
      if (!external_event)
      {
        delete data_ptr_tmp;
      }
      data_ptr_tmp = nullptr;
    }
    external_event = false;
#else
    if (data_ptr_tmp)
    {
      delete data_ptr_tmp;
      data_ptr_tmp = nullptr;
    }
#endif
  }
}
