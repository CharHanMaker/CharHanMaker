#ifndef LPF_H
#define LPF_H

#include <Arduino.h>
#include "timer.h"
class LPF {
  private:
    float timeConstant;
    float prevValue;
    timer tim;

  public:
    LPF(float _timeConstant);
    float update(float _value);
};

#endif