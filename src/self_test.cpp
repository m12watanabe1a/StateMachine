#include <self_test.hpp>
#include <iostream>

SelfTest::SelfTest(uint8_t max_states) : StateMachine(max_states)
{
}

void SelfTest::cancel()
{
  PARENT_TRANSITION(ST_FAILED)

  static const uint8_t TRANSITIONS[] = {
      TRANSITION_MAP_ENTRY(EVENT_IGNORED) // ST_IDLE
      TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // ST_COMPLETED
      TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // ST_FAILED
  };
  assert(this->getCurrentState() < ST_MAX_STATES);
  this->externalEvent(TRANSITIONS[this->getCurrentState()], nullptr);
  static_assert(
      (sizeof(TRANSITIONS) / sizeof(uint8_t)) == ST_MAX_STATES,
      "Invalid sizeof TRANSITIONS");
}

STATE_DEFINE(SelfTest, Idle, NoEventData)
{
  (void)data;
  std::cout << "SelfTest::ST_Idle" << std::endl;
}

ENTRY_DEFINE(SelfTest, EntryIdle, NoEventData)
{
  (void)data;
  std::cout << "SelfTest::EntryIdle" << std::endl;
}

STATE_DEFINE(SelfTest, Completed, NoEventData)
{
  (void)data;
  std::cout << "SelfTest::Completed" << std::endl;
}

STATE_DEFINE(SelfTest, Failed, NoEventData)
{
  (void)data;
  std::cout << "SelfTest::Failed" << std::endl;
  this->internalEvent(ST_IDLE);
}