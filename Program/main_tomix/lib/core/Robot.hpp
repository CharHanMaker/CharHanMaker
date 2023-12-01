#ifndef __Robot__
#define __Robot__

#include "core.hpp"
#include "MyMath.hpp"
#include "TickTwo.h"
#include "Motor.hpp"
#include <Wire.h>
#include <timer.h>
#include "LPF.hpp"
#include "MultipleAS5600.hpp"

typedef struct {
    uint16_t angleLeft;    //[deg]
    uint16_t angleRight;   //[deg]
    float angulerVelLeft;  //[rad/s]
    float angulerVelRight; //[rad/s]
} sensors_t;

class Robot {
  public:
    Robot() {}
    timer Time;
    MultipleAS5600 AbsEncorders = MultipleAS5600(Wire, 0x70, 2);
    Motor motorA = Motor(CorePins::MotorPHA, CorePins::MotorENA, &AbsEncorders, 0);
    Motor motorB = Motor(CorePins::MotorPHB, CorePins::MotorENB, &AbsEncorders, 1);
    sensors_t sensors;

    void deviceBegin() {
        // シリアル通信の設定
        Serial.begin(115200);

        // I2Cの設定
        Wire.setClock(400000); // I2C の通信速度 : 400k Hz
        Wire.begin();
        AbsEncorders.begin();
        AbsEncorders.setDirection(0, CCW);
        AbsEncorders.setDirection(1, CCW);

        // ピンの設定
        pinMode(CorePins::ENG_SW, INPUT);
        pinMode(CorePins::SW, INPUT);
        pinMode(CorePins::Debug_LED, OUTPUT);
        pinMode(CorePins::Alive_LED, OUTPUT);

        // モーターのPWM周波数を設定
        // analogWriteFreq(10000);
        // analogWriteRange(65535);
        motorA.setSaturation(65535, -65535);
        motorA.begin();
    }
};

TickTwo alive(
    []() {
        digitalWrite(CorePins::Alive_LED, !digitalRead(CorePins::Alive_LED));
    },
    500, 0, MILLIS);

void setup1() {
    alive.start();
}

void loop1() {
    alive.update();
}
#endif