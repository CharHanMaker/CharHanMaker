#ifndef __Robot__
#define __Robot__

#include "core.hpp"
#include "MyMath.hpp"
#include "TickTwo.h"
// #include "Motor.hpp"1
#include <Wire.h>
#include <timer.h>
#include "LPF.hpp"
#include "MultipleAS5600.hpp"

class Devices {
  public:
    Devices()
        : AbsEncorders(Wire, 0x70, 2) {}
    timer Time;
    MultipleAS5600 AbsEncorders;
    uint8_t a = 0;

    void deviceBegin() {
        AbsEncorders.begin();
    }
};

#endif
