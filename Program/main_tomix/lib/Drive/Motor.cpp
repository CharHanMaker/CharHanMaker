#include "Motor.hpp"

Motor::Motor(uint8_t _ph, uint8_t _en) : ph(_ph), en(_en), Speed(0) {}

void Motor::begin() {
    analogWriteFreq(10000); // 10KHzがちょうど良さそう
    analogWriteResolution(65535);

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
    analogWrite(ph, 32767);
    digitalWrite(en, HIGH);
}

void Motor::drive(float radPerSec) {
    // PIDする
}
