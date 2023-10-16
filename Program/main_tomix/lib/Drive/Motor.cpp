#include "Motor.hpp"

Motor::Motor(uint8_t __pin, PioEncoder *_encorder) {
    pin = __pin;
    encorder = _encorder;
}

void Motor::begin() {
    pinMode(pin, OUTPUT);
    pinMode(CorePins::Motor2, OUTPUT);
    analogWriteFreq(100 * 10E3);
    analogWriteRange(4096);
    encorder->begin();
    dt.reset();
}

void Motor::drive(int rpm) {
    // モータを回す関数
    // rpm: 回転数
    encorder->getCount();
}

void Motor::setGain(float _PGain, float _IGain, float _DGain) {
    P = _PGain;
    I = _IGain;
    D = _DGain;
}

void Motor::setLimit(float _limitUpper, float _limitLower) {
    limitUpper = _limitUpper;
    limitLower = _limitLower;
}