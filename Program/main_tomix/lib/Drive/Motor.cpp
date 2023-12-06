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