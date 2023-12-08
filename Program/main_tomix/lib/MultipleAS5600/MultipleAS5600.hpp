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
#define BIT_12_TO_RADIAN 0.00153435538
#define BIT_12_TO_DEGREE 0.08791208791
#define VALUE_ERROR 4097

class MultipleAS5600 {
  public:
    MultipleAS5600(TwoWire &_i2c, uint8_t _muxAddress, uint8_t _sensorQty);
    void begin();

    // 角度読み取り系
    uint16_t readRawValue(uint8_t _sensorNumber);   // 12bit
    uint16_t read12BitValue(uint8_t _sensorNumber); // 12bit
    float readDegree(uint8_t _sensorNumber);        // degree
    float readRadian(uint8_t _sensorNumber);        // radian

    float getVelocity(uint8_t _sensorNumber); // rad/s

    float getContinuousDegree(uint8_t _sensorNumber); // 連続した角度を返す．はず[deg]
    float getContinuousRadian(uint8_t _sensorNumber); // 連続した角度を返す．はず[rad]

    void setDirection(uint8_t _sensorNumber, bool cw);       // エンコーダの返す方向の符号を設定
    float setZero(uint8_t _sensorNumber);                    // 角度の初期位置をセット
    float setZero(uint8_t _sensorNumber, uint16_t rawValue); // 角度の初期位置をセット

    int16_t getRotationCount(uint8_t _sensorNumber); // 回転数を返す．
    void resetRotationCount(uint8_t _sensorNumber);  // 回転数をリセットする．
  private:
    uint8_t muxAddress;
    uint8_t sensorQty;
    uint8_t sensorNumber;
    uint8_t prevPort;

    int16_t valueArray[8] = {0};    // [DEG]
    int16_t valuePrev[8] = {0};     // [DEG]
    int16_t valueZero[8] = {0};     //  [DEG]
    float continuousAngle[8] = {0}; // [DEG]
    uint16_t rawValue[8] = {0};
    int16_t count[8] = {0};
    bool isCW[8] = {true};
    float dt[8] = {0};

    TwoWire *i2c;
    timer velTimer[8];
    LPF velocityLPF[8] = {LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF, LPF_COEFF};
};

#endif