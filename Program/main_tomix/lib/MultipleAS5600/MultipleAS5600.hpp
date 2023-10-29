#ifndef __MULTIPLEAS5600_HPP__
#define __MULTIPLEAS5600_HPP__

#include "Arduino.h"
#include "Wire.h"
#include "timer.h"
#include "LPF.hpp"

#define MUX_ADDR 0x70    // TCA9548A default address
#define AS5600_ADDR 0x36 // AS5600 default address
#define LPF_COEFF 0.02   // LPF coefficient

class MultipleAS5600 {
  public:
    MultipleAS5600(TwoWire &_i2c, uint8_t _muxAddress, uint8_t _sensorQty);
    void begin();
    uint16_t readDegree(uint8_t _sensorNumber);
    void readDegreeAll(uint16_t *_angleArray);
    float getVelocity(uint8_t _sensorNumber);

  private:
    uint8_t muxAddress;
    uint8_t sensorQty;
    uint8_t sensorNumber;
    uint8_t prevPort;
    uint16_t angleArray[8];
    float shAnglePrev[8];
    TwoWire *i2c;
    timer velTimer[8];
    LPF velocityLPF[8] = {LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF};
};

#endif