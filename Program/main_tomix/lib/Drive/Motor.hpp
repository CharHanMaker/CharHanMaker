#ifndef __Motor__
#define __Motor__

#include "Motor.hpp"
#include "core.hpp"
#include "timer.h"

class Motor {
  public:
    Motor(uint8_t __pin);
    void begin();
    void drive(int rpm);
    void setGain(float _PGain, float _IGain, float _DGain);
    void setLimit(float _limitUpper, float _limitLower);
    void testSineWave();

  private:
    uint8_t pin;
    float limitUpper, limitLower;
    float P, I, D;
    timer dt;
};

#endif