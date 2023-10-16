#ifndef LPF_H
#define LPF_H

#include <Arduino.h>

class LPF {
  private:
    float timeConstant;
    float prevValue;
    Timer timer;

  public:
    LPF(float _timeConstant);
    float update(float _value);
};

#endif