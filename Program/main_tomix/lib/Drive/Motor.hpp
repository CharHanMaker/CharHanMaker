#ifndef __MOTOR__
#define __MOTOR__

#include "Arduino.h"
#include "PID.hpp"
#include "MultipleAS5600.hpp"
class Motor {
  public:
    // @brief コンストラクタ
    // @param _ph モータの回転方向を制御するピン
    // @param _en モータの回転速度を制御するピン
    // @param _encoder エンコーダのインスタンス
    // @param encoderPort エンコーダのポート番号
    Motor(uint8_t _ph, uint8_t _en, MultipleAS5600 *_encoder, uint8_t encoderPort);
    void begin();
    void setSaturation(int32_t _max, int32_t _min);
    void runOpenloop(int32_t pwmDutyCount, bool brakeOnZero);
    void stop();
    void brake();

  private:
    uint8_t ph;
    uint8_t en;
    int32_t max;
    int32_t min;
    int16_t pwmResolution = 65535;
    PID pid = PID(0, 0, 0, 0);
    MultipleAS5600 *encoder;
    uint8_t encoderPort;

    int32_t pwmDutyCountPrev;
};

#endif
