#ifndef __Motor__
#define __Motor__

#include "Motor.hpp"
#include "core.hpp"
#include "pio_encoder.h"
#include "timer.h"

class Motor {
  public:
    Motor(uint8_t __pin, PioEncoder *_encorder);
    void begin();
    void drive(int rpm);
    void setGain(float _PGain, float _IGain, float _DGain);
    void setLimit(float _limitUpper, float _limitLower);

  private:
    uint8_t pin;
    PioEncoder *encorder;
    float limitUpper, limitLower;
    float P, I, D;
    timer dt;
};

#endif