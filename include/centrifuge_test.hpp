#pragma once

#include <self_test.hpp>

class CentrifugeTest : public SelfTest
{
public:
  CentrifugeTest();

  virtual void start();
  void poll();

  bool isPollActive() { return this->poll_active_; }

private:
  bool poll_active_;
  int32_t speed_;

  void startPoll() { this->poll_active_ = true; }
  void stopPoll() { this->poll_active_ = false; }

  enum states
  {
    ST_START_TEST = SelfTest::ST_MAX_STATES,
    ST_ACCELERATION,
    ST_WAIT_FOR_ACCELERATION,
    ST_DECELERATION,
    ST_WAIT_FOR_DECELERATION,
    ST_MAX_STATES
  };

  STATE_DECLARE(CentrifugeTest, Idle, NoEventData)
  STATE_DECLARE(CentrifugeTest, StartTest, NoEventData)
  GUARD_DECLARE(CentrifugeTest, GuardStartTest, NoEventData)
  STATE_DECLARE(CentrifugeTest, Acceleration, NoEventData)
  STATE_DECLARE(CentrifugeTest, WaitForAcceleration, NoEventData)
  EXIT_DECLARE(CentrifugeTest, ExitWaitForAcceleration)
  STATE_DECLARE(CentrifugeTest, Deceleration, NoEventData)
  STATE_DECLARE(CentrifugeTest, WaitForDeceleration, NoEventData)
  EXIT_DECLARE(CentrifugeTest, ExitWaitForDeceleration)

private:
  virtual const StateMapRow *getStateMap() { return nullptr; }
  virtual const StateMapRowEx *getStateMapEx()
  {
    static const StateMapRowEx STATE_MAP[] = {
        STATE_MAP_ENTRY_ALL_EX(&Idle, nullptr, &EntryIdle, nullptr),
        STATE_MAP_ENTRY_EX(&Completed),
        STATE_MAP_ENTRY_EX(&Failed),
        STATE_MAP_ENTRY_ALL_EX(&StartTest, &GuardStartTest, nullptr, nullptr),
        STATE_MAP_ENTRY_EX(&Acceleration),
        STATE_MAP_ENTRY_ALL_EX(&WaitForAcceleration, nullptr, nullptr, &ExitWaitForAcceleration),
        STATE_MAP_ENTRY_EX(&Deceleration),
        STATE_MAP_ENTRY_ALL_EX(&WaitForDeceleration, nullptr, nullptr, &ExitWaitForDeceleration)
    };
    static_assert(
        (sizeof(STATE_MAP) / sizeof(StateMapRowEx)) == ST_MAX_STATES,
        "Invalid size of STATE_MAP");
    return &STATE_MAP[0];
  }
};
