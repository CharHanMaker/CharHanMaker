#ifndef __MULTIPLEAS5600_HPP__
#define __MULTIPLEAS5600_HPP__

#include "Arduino.h"
#include "Wire.h"

#define MUX_ADDR 0x70    // TCA9548A default address
#define AS5600_ADDR 0x36 // AS5600 default address

class MultipleAS5600 {
  public:
    MultipleAS5600(TwoWire &_i2c, uint8_t _muxAddress, uint8_t _sensorQty);
    void begin();
    uint16_t readDegree(uint8_t _sensorNumber);

  private:
    uint8_t muxAddress;
    uint8_t sensorQty;
    uint8_t sensorNumber;
    uint8_t prevPort;
    TwoWire *i2c;
};

#endif