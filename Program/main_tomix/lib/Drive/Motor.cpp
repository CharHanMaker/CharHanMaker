#include "Motor.hpp"

Motor::Motor(uint8_t _ph, uint8_t _en) : ph(_ph), en(_en), Speed(0), pwmDutyCount(0) {}

void Motor::begin() {
    analogWriteResolution(65535);
    analogWriteFreq(10000);
    pinMode(ph, OUTPUT);
    pinMode(en, OUTPUT);
}

void Motor::setSaturation(uint16_t _max, uint16_t _min) {
    max = _max;
    min = _min;
}

void Motor::runOpenloop(uint16_t pwmDutyCount) {
    constrain(pwmDutyCount, 0, 65535);
    if (pwmDutyCount > 0) {
        digitalWrite(en, HIGH);
    } else {
        digitalWrite(en, LOW);
    }
    analogWrite(ph, abs(pwmDutyCount));
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
