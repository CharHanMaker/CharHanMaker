#include <MotorPID.hpp>

void MotorPid::setVelPIDGain(float _p, float _i, float _d) {
    pidVel.setGain(_p, _i, _d);
}

void MotorPid::setAnglePIDGain(float _p, float _i, float _d) {
    pidAngle.setGain(_p, _i, _d);
}

void MotorPid::setSynchronizePIDGain(float _p, float _i, float _d) {
    pidSynchronize.setGain(_p, _i, _d);
}

void MotorPid::setVelPIDLimit(float _limitMin, float _limitMax) {
    pidVel.setLimit(_limitMin, _limitMax);
}

void MotorPid::setAnglePIDLimit(float _limitMin, float _limitMax) {
    pidAngle.setLimit(_limitMin, _limitMax);
}

void MotorPid::setSynchronizePIDLimit(float _limitMin, float _limitMax) {
    pidSynchronize.setLimit(_limitMin, _limitMax);
}

void MotorPid::resetPID() {
    pidVel.reset();
    pidAngle.reset();
}

float MotorPid::velControl(float radPerSec) { // 目標角速度rad/sが引数のPID制御関数
    encoder->readRadian(encoderPort);
    velAngular = encoder->getVelocity(encoderPort);
    error = radPerSec - velAngular;

    pidVel.appendError(error);
    pidVel.compute();
    return out = pidVel.getPID();

    // Motor::runOpenloop(out); // これは外に出す
}

float MotorPid::angleControl(float rad) { // 目標角radが引数のPID制御関数
    angle = encoder->readRadian(encoderPort);
    error = rad - angle;

    pidAngle.appendError(error);
    pidAngle.compute();
    return out = pidAngle.getPID();
}

float MotorPid::synchronizeControl(float radMaster, float radSlave) { // モータ同期のためのPID制御関数
    error = radMaster - radSlave;

    pidSynchronize.appendError(error);
    pidSynchronize.compute();
    return out = pidSynchronize.getPID();
}