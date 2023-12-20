#ifndef __MOTORPID__
#define __MOTORPID__

#include <Motor.hpp>

class MotorPid : public Motor { // Motorクラスを継承
  public:
    MotorPid(uint8_t _ph, uint8_t _en, MultipleAS5600 *_encoder, uint8_t encoderPort) : Motor(_ph, _en, _encoder, encoderPort) {}
    void setVelPIDGain(float _p, float _i, float _d);
    void setAnglePIDGain(float _p, float _i, float _d);
    void setSynchronizePIDGain(float _p, float _i, float _d);
    void setVelPIDLimit(float _limitMin, float _limitMax);
    void setAnglePIDLimit(float _limitMin, float _limitMax);
    void setSynchronizePIDLimit(float _limitMin, float _limitMax);
    void resetPID();

    float velControl(float radPerSec);
    float angleControl(float rad);                             // 引数は目標値, PIDを通して電圧を返す
    float synchronizeControl(float radMaster, float radSlave); //

  private:
    uint8_t ph;
    uint8_t en;
    int32_t max;
    int32_t min;
    int16_t pwmResolution = 65535;
    PID pidVel = PID(0, 0, 0, 0);
    PID pidAngle = PID(0, 0, 0, 0);
    PID pidSynchronize = PID(0, 0, 0, 0);
    MultipleAS5600 *encoder;
    uint8_t encoderPort;

    float error;
    float velAngular;
    float angle;
    float out;
};

#endif