#ifndef __MOTOR__
#define __MOTOR__

#include "Arduino.h"
class Motor {
  public:
    Motor(uint8_t _ph, uint8_t _en);
    void begin();
    void setSaturation(uint16_t _max, uint16_t _min);
    void runOpenloop(uint16_t pwmDutyCount);
    void brake();
    void stop();
    void drive(float radPerSec);

  private:
    float Speed;
    uint8_t ph;
    uint8_t en;
    uint16_t max;
    uint16_t min;
    uint16_t pwmDutyCount;
};

#endif
