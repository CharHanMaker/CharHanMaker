#ifndef __MOTOR__
#define __MOTOR__

#include "Arduino.h"
class Motor {
  public:
    Motor(uint8_t _ph, uint8_t _en);
    void begin();
    void setSaturation(int32_t _max, int32_t _min);
    void runOpenloop(int32_t pwmDutyCount, bool brakeOnZero);
    void stop();
    void brake();
    void drive(float radPerSec);

  private:
    float Speed;
    uint8_t ph;
    uint8_t en;
    int32_t max;
    int32_t min;
};

#endif
