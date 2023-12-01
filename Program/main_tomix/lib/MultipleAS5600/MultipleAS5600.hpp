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
    void readDegreeAll(uint16_t *_shaftAngleArray);
    float getVelocity(uint8_t _sensorNumber); // rad/s
    float getContinuousRadians(uint8_t _sensorNumber); // 連続した角度を返す
    void setDirection(uint8_t _sensorNumber, bool cw); //エンコーダの返す方向の符号を設定
    void setZero(uint8_t _sensorNumber); // 角度の初期位置をセット
  private:
    uint8_t muxAddress;
    uint8_t sensorQty;
    uint8_t sensorNumber;
    uint8_t prevPort;
    float shaftAngleArray[8];// [DEG]
    float shaftAnglePrev[8]; // [DEG]
    float shaftAngleZero[8]; //  [DEG]
    bool isCW[8];
    TwoWire *i2c;
    timer velTimer[8];
    LPF velocityLPF[8] = {LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF};
};

#endif