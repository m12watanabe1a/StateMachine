#pragma once

#include <state_machine.hpp>

class SelfTest : public StateMachine
{
public:
  SelfTest(uint8_t max_state_);
  virtual void start() = 0;
  void cancel();

protected:
  enum states
  {
    ST_IDLE,
    ST_COMPLETED,
    ST_FAILED,
    ST_MAX_STATES,
  };

  STATE_DECLARE(SelfTest, Idle, NoEventData)
  ENTRY_DECLARE(SelfTest, EntryIdle, NoEventData)
  STATE_DECLARE(SelfTest, Completed, NoEventData)
  STATE_DECLARE(SelfTest, Failed, NoEventData)
};