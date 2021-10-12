#include "motor.hpp"

#include <iostream>

using namespace std;

Motor::Motor() : StateMachine(ST_MAX_STATES),
                 current_speed_(0)
{
}

// set motor speed external event
void Motor::setSpeed(MotorData *data)
{
  static const uint8_t TRANSITIONS[] = {
      TRANSITION_MAP_ENTRY(ST_START)        // ST_IDLE
      TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)   // ST_STOP
      TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED) // ST_START
      TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED) // ST_CHANGE_SPEED
  };
  END_TRANSITION_MAP(data)
}

// halt motor external event
void Motor::halt()
{
  static const uint8_t TRANSITIONS[] = {
      TRANSITION_MAP_ENTRY(EVENT_IGNORED) // ST_IDLE
      TRANSITION_MAP_ENTRY(CANNOT_HAPPEN) // ST_STOP
      TRANSITION_MAP_ENTRY(ST_STOP)       // ST_START
      TRANSITION_MAP_ENTRY(ST_STOP)       // ST_CHANGE_SPEED
  };
  END_TRANSITION_MAP(NULL)
}

// state machine sits here when motor is not running
STATE_DEFINE(Motor, Idle, NoEventData)
{
  (void)data; // cast to avoid gcc unused warning
  cout << "Motor::ST_Idle" << endl;
}

// stop the motor
STATE_DEFINE(Motor, Stop, NoEventData)
{
  (void)data; // cast to avoid gcc unused warning
  cout << "Motor::ST_Stop" << endl;
  this->current_speed_ = 0;
  this->internalEvent(ST_IDLE);
}

// start the motor going
STATE_DEFINE(Motor, Start, MotorData)
{
  cout << "Motor::ST_Start : Speed is " << data->speed << endl;
  this->current_speed_ = data->speed;
}

// changes the motor speed once the motor is moving
STATE_DEFINE(Motor, ChangeSpeed, MotorData)
{
  cout << "Motor::ST_ChangeSpeed : Speed is " << data->speed << endl;
  this->current_speed_ = data->speed;
}
