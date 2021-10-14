#include <centrifuge_test.hpp>
#include <iostream>

CentrifugeTest::CentrifugeTest() : SelfTest(ST_MAX_STATES),
                                   poll_active_(false),
                                   speed_(0)
{
}

void CentrifugeTest::start()
{
  static const uint8_t TRANSITIONS[] = {
      ST_START_TEST, // ST_IDLE
      CANNOT_HAPPEN, // ST_COMPLETED
      CANNOT_HAPPEN, // ST_FAILED
      EVENT_IGNORED, // ST_START_TEST
      EVENT_IGNORED, // ST_ACCELERATION
      EVENT_IGNORED, // ST_WAIT_FOR_ACCELERATION
      EVENT_IGNORED, // ST_DECELERATION
      EVENT_IGNORED, // ST_WAIT_FOR_DECELERATION
  };
  assert(this->getCurrentState() < ST_MAX_STATES);
  this->externalEvent(TRANSITIONS[this->getCurrentState()], nullptr);
  static_assert(
      (sizeof(TRANSITIONS) / sizeof(uint8_t)) == ST_MAX_STATES,
      "Invalid size of TRANSITIONS");
}

void CentrifugeTest::poll()
{
  static const uint8_t TRANSITIONS[] = {
      EVENT_IGNORED,            // ST_IDLE
      EVENT_IGNORED,            // ST_COMPLETED
      EVENT_IGNORED,            // ST_FAILED
      EVENT_IGNORED,            // ST_START_TEST
      ST_WAIT_FOR_ACCELERATION, // ST_ACCELERATION
      ST_WAIT_FOR_ACCELERATION, // ST_WAIT_FOR_ACCELERATION
      ST_WAIT_FOR_DECELERATION, // ST_DECELERATION
      ST_WAIT_FOR_DECELERATION, // ST_WAIT_FOR_DECELERATION
  };
  assert(this->getCurrentState() < ST_MAX_STATES);
  this->externalEvent(TRANSITIONS[this->getCurrentState()], nullptr);
  static_assert(
      (sizeof(TRANSITIONS) / sizeof(uint8_t)) == ST_MAX_STATES,
      "Invalid size of TRANSITIONS");
}

STATE_DEFINE(
    CentrifugeTest,
    Idle,
    NoEventData)
{
  (void)data;
  std::cout << "CentrifugeTest::ST_Idle" << std::endl;

  SelfTest::ST_Idle(data);
  this->stopPoll();
}

STATE_DEFINE(
    CentrifugeTest,
    StartTest,
    NoEventData)
{
  (void)data;
  std::cout << "CentrifugeTest::GD_GuardStartTest" << std::endl;
  this->internalEvent(ST_ACCELERATION);
}

GUARD_DEFINE(
    CentrifugeTest,
    GuardStartTest,
    NoEventData)
{
  (void)data;
  std::cout << "CentrifugeTest::GD_GuardStartTest" << std::endl;
  if (this->speed_ == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

STATE_DEFINE(
    CentrifugeTest,
    Acceleration,
    NoEventData)
{
  (void)data;
  std::cout << "CentrifugeTest::ST_Acceleration" << std::endl;
  this->startPoll();
}

STATE_DEFINE(
    CentrifugeTest,
    WaitForAcceleration,
    NoEventData)
{
  (void)data;
  std::cout << "CentrifugeTest::ST_WaitForAcceleration : Speed is " << this->speed_ << std::endl;
  if (++this->speed_ >= 5)
  {
    this->internalEvent(ST_DECELERATION);
  }
}

EXIT_DEFINE(
    CentrifugeTest,
    ExitWaitForAcceleration)
{
  std::cout << "CentrifugeTest::EX_ExitWaitForAcceleration" << std::endl;
  this->stopPoll();
}

STATE_DEFINE(
    CentrifugeTest,
    Deceleration,
    NoEventData)
{
  (void)data;
  std::cout << "CentrifugeTest::ST_Deceleration" << std::endl;
  this->startPoll();
}

STATE_DEFINE(
    CentrifugeTest,
    WaitForDeceleration,
    NoEventData)
{
  (void)data;
  std::cout << "CentrifugeTest::ST_WaitForDeceleration : Speed is " << this->speed_ << std::endl;
  if (this->speed_-- == 0)
  {
    this->internalEvent(ST_COMPLETED);
  }
}

EXIT_DEFINE(
    CentrifugeTest,
    ExitWaitForDeceleration)
{
  std::cout << "CentrifugeTest::EX_ExitWaitForDeceleration" << std::endl;
  this->stopPoll();
}