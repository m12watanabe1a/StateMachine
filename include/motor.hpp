#ifndef _MOTOR_H
#define _MOTOR_H

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

  // External events taken by this state machine
  void SetSpeed(MotorData *data);
  void Halt();

private:
  int m_currentSpeed;

  // State enumeration order must match the order of state method entries
  // in the state map.
  enum States
  {
    ST_IDLE,
    ST_STOP,
    ST_START,
    ST_CHANGE_SPEED,
    ST_MAX_STATES
  };

  // Define the state machine state functions with event data type
  STATE_DECLARE(Motor, Idle, NoEventData)
  STATE_DECLARE(Motor, Stop, NoEventData)
  STATE_DECLARE(Motor, Start, MotorData)
  STATE_DECLARE(Motor, ChangeSpeed, MotorData)

  // State map to define state object order. Each state map entry defines a
  // state object.
private:
  virtual const StateMapRowEx *GetStateMapEx() { return nullptr; }
  virtual const StateMapRow *GetStateMap()
  {
    static const StateMapRow STATE_MAP[]{
        &Idle,
        &Stop,
        &Start,
        &ChangeSpeed,
    };
    static_assert((sizeof(STATE_MAP) / sizeof(StateMapRow)) == ST_MAX_STATES);
    return &STATE_MAP[0];
  }
};

#endif