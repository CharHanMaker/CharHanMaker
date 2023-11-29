#include "Motor.hpp"

Motor::Motor(uint8_t _ph, uint8_t _en, MultipleAS5600 *_encoder, uint8_t _encoderPort)
    : ph(_ph), en(_en),
      encoder(_encoder),
      encoderPort(_encoderPort) {}

void Motor::begin() {
    analogWriteFreq(10000); // 10KHzがちょうど良さそう
    analogWriteResolution(pwmResolution);

    pinMode(ph, OUTPUT);
    pinMode(en, OUTPUT);
}

void Motor::setSaturation(int32_t _max, int32_t _min) {
    max = _max;
    min = _min;
}

void Motor::runOpenloop(int32_t pwmDutyCount, bool brakeOnZero = false) {
    if (brakeOnZero && pwmDutyCount == 0) {
        brake();
        return;
    }
    if (pwmDutyCount > max) {
        pwmDutyCount = max;
    } else if (pwmDutyCount < min) {
        pwmDutyCount = min;
    }
    if (pwmDutyCount > 0) {
        digitalWrite(ph, HIGH);
    } else {
        digitalWrite(ph, LOW);
    }
    analogWrite(en, abs(pwmDutyCount));
}

void Motor::stop() {
    digitalWrite(ph, LOW);
    digitalWrite(en, LOW);
}

void Motor::brake() {
    analogWrite(ph, pwmResolution / 2);
    digitalWrite(en, HIGH);
}

void Motor::drive(float radPerSec) {
    //: TODO: PID制御
    // @ryoskRFR 速度のPID制御を追加してもろて
    encoder->readDegree(encoderPort);               // よしなにして，つかわなくてもいいよ
    velAngular = encoder->getVelocity(encoderPort); // よしなにして，つかわなくてもいいよ
    float error = radPerSec - velAngular;           // よしなにして，つかわなくてもいいよ
    pid.appendError(error);                         // よしなにして，つかわなくてもいいよ
    pid.compute();                                  // よしなにして，つかわなくてもいいよ
    uint32_t out = pid.getPID();                    // よしなにして，つかわなくてもいいよ
    runOpenloop(out);                               // よしなにして，つかわなくてもいいよ
}

void Motor::setPIDGain(float _p, float _i, float _d) {
    pid.setGain(_p, _i, _d);
}

void Motor::setPIDLimit(float _limitMin, float _limitMax) {
    pid.setLimit(_limitMin, _limitMax);
}

void Motor::resetPID() {
    pid.reset();
}