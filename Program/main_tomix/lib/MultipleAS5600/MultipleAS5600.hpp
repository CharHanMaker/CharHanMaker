#ifndef __MULTIPLEAS5600_HPP__
#define __MULTIPLEAS5600_HPP__

#include "Arduino.h"
#include "Wire.h"
#include "timer.h"
#include "LPF.hpp"
#include "MyMath.hpp"

#define MUX_ADDR 0x70    // TCA9548A default address
#define AS5600_ADDR 0x36 // AS5600 default address
#define LPF_COEFF 0.05   // LPF coefficient
#define BIT_12_TO_RADIAN 651.739491961

class MultipleAS5600 {
  public:
    MultipleAS5600(TwoWire &_i2c, uint8_t _muxAddress, uint8_t _sensorQty);
    void begin();
    float readDegree(uint8_t _sensorNumber); // degree
    float readRadian(uint8_t _sensorNumber); // radian
    void readDegreeAll(uint16_t *_angleArray);
    float getVelocity(uint8_t _sensorNumber); // rad/s
    void setDirection(uint8_t _sensorNumber, bool cw);
    void setZero(uint8_t _sensorNumber);

  private:
    uint8_t muxAddress;
    uint8_t sensorQty;
    uint8_t sensorNumber;
    uint8_t prevPort;
    float angleArray[8];
    float shAnglePrev[8];
    float shAngleZero[8];
    bool isCW[8];
    TwoWire *i2c;
    timer velTimer[8];
    LPF velocityLPF[8] = {LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF};
};

#endif