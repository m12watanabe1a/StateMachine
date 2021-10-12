#pragma once

#include "state_machine.hpp"

class MotorData : public EventData
{
public:
  int speed;
};

class Motor : public StateMachine
{
public:
  Motor();

  // External event
  void setSpeed(MotorData *data);
  void halt();

private:
  int current_speed_;

  enum States
  {
    ST_IDLE,
    ST_STOP,
    ST_START,
    ST_CHANGE_SPEED,
    ST_MAX_STATES
  };

  // States
  STATE_DECLARE(Motor, Idle, NoEventData)
  STATE_DECLARE(Motor, Stop, NoEventData)
  STATE_DECLARE(Motor, Start, MotorData)
  STATE_DECLARE(Motor, ChangeSpeed, MotorData)

private:
  // State map
  virtual const StateMapRowEx *getStateMapEx() { return nullptr; }
  virtual const StateMapRow *getStateMap()
  {
    static const StateMapRow STATE_MAP[]{
        &Idle,
        &Stop,
        &Start,
        &ChangeSpeed,
    };
    static_assert(
        (sizeof(STATE_MAP) / sizeof(StateMapRow)) == ST_MAX_STATES,
        "Invalid size of STATE_MAP");
    return &STATE_MAP[0];
  }
};
