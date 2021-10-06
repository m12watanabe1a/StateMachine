#ifndef _MOTOR_H
#define _MOTOR_H
#include "state_machine.hpp"

// structure to hold event data passed into state machine
struct MotorData : public EventData
{
  int speed;
};

// the Motor state machine class
class Motor : public StateMachine
{
public:
  Motor() : StateMachine(ST_MAX_STATES) {}

  // external events taken by this state machine
  void Halt();
  void SetSpeed(MotorData *);

private:
  // state machine state functions
  void ST_Idle(EventData *);
  void ST_Stop(EventData *);
  void ST_Start(MotorData *);
  void ST_ChangeSpeed(MotorData *);

  // state map to define state function order
public:
  const StateStruct *GetStateMap()
  {
    static const StateStruct StateMap[] = {
        {reinterpret_cast<StateFunc>(&Motor::ST_Idle)},
        {reinterpret_cast<StateFunc>(&Motor::ST_Stop)},
        {reinterpret_cast<StateFunc>(&Motor::ST_Start)},
        {reinterpret_cast<StateFunc>(&Motor::ST_ChangeSpeed)},
    };

    return &StateMap[0];
  }

  // state enumeration order must match the order of state
  // method entries in the state map
  enum E_States
  {
    ST_IDLE = 0,
    ST_STOP,
    ST_START,
    ST_CHANGE_SPEED,
    ST_MAX_STATES
  };
};
#endif // _MOTOR_H
