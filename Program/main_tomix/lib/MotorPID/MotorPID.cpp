#include <MotorPID.hpp>

void MotorPid::setVelPIDGain(float _p, float _i, float _d) {
    pidVel.setGain(_p, _i, _d);
}

void MotorPid::setAnglePIDGain(float _p, float _i, float _d) {
    pidAngle.setGain(_p, _i, _d);
}

void MotorPid::setVelPIDLimit(float _limitMin, float _limitMax) {
    pidVel.setLimit(_limitMin, _limitMax);
}

void MotorPid::setAnglePIDLimit(float _limitMin, float _limitMax) {
    pidAngle.setLimit(_limitMin, _limitMax);
}

void MotorPid::resetPID() {
    pidVel.reset();
    pidAngle.reset();
}

void MotorPid::velControl(float radPerSec) { // 目標値
    //: TODO: PID制御
    // @ryoskRFR 速度のPID制御を追加してもろて
    encoder->readRadian(encoderPort);
    velAngular = encoder->getVelocity(encoderPort);
    error = radPerSec - velAngular;

    pidVel.appendError(error);
    pidVel.compute();
    out = pidVel.getPID();

    Motor::runOpenloop(out); // これは外に出す
}

void MotorPid::angleControl(float rad) { // 引数は目標値
    angle = encoder->readRadian(encoderPort);
    error = rad - angle;

    pidAngle.appendError(error);
    pidAngle.compute();
    out = pidAngle.getPID();
}