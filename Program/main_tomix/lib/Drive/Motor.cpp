#include "Motor.hpp"

Motor::Motor(uint8_t _ph, uint8_t _en) : ph(_ph), en(_en), Speed(0), pwmDutyCount(0) {}

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

void Motor::runOpenloop(uint16_t pwmDutyCount) {
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

void Motor::brake() {
    analogWrite(ph, 65535);
    digitalWrite(en, HIGH);
}

void Motor::stop() {
    digitalWrite(ph, LOW);
    digitalWrite(en, LOW);
}

void Motor::drive(float radPerSec) {
    // PIDする
}
