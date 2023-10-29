#include "LPF.hpp"

LPF::LPF(float _timeConstant) : timeConstant(_timeConstant), prevValue(0) {
    tim.reset();
}

float LPF::update(float _value) {
    float dt = (float)tim.read_us() / 1000000;
    tim.reset();
    float alpha = dt / (timeConstant + dt);
    float value = alpha * _value + (1 - alpha) * prevValue;
    prevValue = value;
    return value;
}