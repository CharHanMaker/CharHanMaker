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

typedef struct {
    uint16_t angleLeft;    //[deg]
    uint16_t angleRight;   //[deg]
    float angulerVelLeft;  //[rad/s]
    float angulerVelRight; //[rad/s]
} sensors_t;

class Robot {
  public:
    Robot() : AbsEncorders(Wire, 0x70, 2) {}
    timer Time;
    MultipleAS5600 AbsEncorders;
    sensors_t sensors;

    void deviceBegin() {
        // シリアル通信の設定
        Serial.begin(115200);

        // I2Cの設定
        Wire.setClock(400000); // I2C の通信速度 : 400k Hz
        Wire.begin();
        AbsEncorders.begin();

        // ピンの設定
        pinMode(CorePins::EncPortA1, INPUT);
        pinMode(CorePins::EncPortB1, INPUT);
        pinMode(CorePins::EncPortA2, INPUT);
        pinMode(CorePins::EncPortB2, INPUT);
        pinMode(CorePins::ENG_SW, INPUT);
        pinMode(CorePins::SW, INPUT);
        pinMode(CorePins::MotorA, OUTPUT);
        pinMode(CorePins::MotorB, OUTPUT);
        pinMode(CorePins::Debug_LED, OUTPUT);
        pinMode(CorePins::Alive_LED, OUTPUT);

        // モーターのPWM周波数を設定
        analogWriteFreq(2000); // 25kHz→2KHz
        analogWriteRange(65535);
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