#ifndef __MOTORPID__
#define __MOTORPID__

#include <Motor.hpp>

class MotorPid : Motor {
  public:
    void setVelPIDGain(float _p, float _i, float _d);
    void setAnglePIDGain(float _p, float _i, float _d);
    void setVelPIDLimit(float _limitMin, float _limitMax);
    void setAnglePIDLimit(float _limitMin, float _limitMax);
    void resetPID();

    void velControl(float radPerSec);
    void angleControl(float rad); // 引数は目標値

  private:
    PID pidVel = PID(0, 0, 0, 0);
    PID pidAngle = PID(0, 0, 0, 0);
    MultipleAS5600 *encoder;
    uint8_t encoderPort;

    float error;
    float velAngular;
    float angle;
    uint32_t out;
};

#endif