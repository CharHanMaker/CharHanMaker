#include "Motor.hpp"

Motor::Motor(uint8_t __pin) {
    pin = __pin;
}

void Motor::begin() {
    pinMode(pin, OUTPUT);
    analogWriteFreq(25000); // 25kHz
    analogWriteRange(65535);
    encorder->begin();
    dt.reset();
}

void Motor::drive(int rpm) {
    // モータを回す関数
    // rpm: 回転数
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

void Motor::testSineWave() {
    i++;
    int power = 65535 / 2 * MyMath::sinDeg(i) + 65535 / 2;
    analogWrite(16, power);
    analogWrite(25, power);
    delay(10);
    Serial.printf("i:%d\n", power);
}