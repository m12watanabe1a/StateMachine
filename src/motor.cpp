#include <assert.h>
#include "motor.hpp"
#include <iostream>

using namespace std;

// halt motor external event
void Motor::Halt(void)
{
  static const unsigned char TRANSITIONS[] = {
      EVENT_IGNORED,
      CANNOT_HAPPEN,
      ST_STOP,
      ST_STOP,
  };
  ExternalEvent(TRANSITIONS[currentState], NULL);
}

// set motor speed external event
void Motor::SetSpeed(MotorData *pData)
{
  static const unsigned char TRANSITIONS[] = {
      ST_START,
      CANNOT_HAPPEN,
      ST_CHANGE_SPEED,
      ST_CHANGE_SPEED,
      0,
  };
  ExternalEvent(TRANSITIONS[currentState], pData);
}

// state machine sits here when motor is not running
void Motor::ST_Idle(EventData *pData)
{
  cout << "Motor::ST_Idle" << endl;
}

// stop the motor
void Motor::ST_Stop(EventData *pData)
{
  cout << "Motor::ST_Stop" << endl;

  // perform the stop motor processing here
  // transition to ST_Idle via an internal event
  InternalEvent(ST_IDLE);
}

// start the motor going
void Motor::ST_Start(MotorData *pData)
{
  cout << "Motor::ST_Start" << endl;
  // set initial motor speed processing here
}

// changes the motor speed once the motor is moving
void Motor::ST_ChangeSpeed(MotorData *pData)
{
  cout << "Motor::ST_ChangeSpeed" << endl;
  // perform the change motor speed to pData->speed here
}
