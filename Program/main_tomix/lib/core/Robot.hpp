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
#include <MotorPID.hpp>

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
    MotorPid motorA = MotorPid(CorePins::MotorPHA, CorePins::MotorENA, &AbsEncorders, 0);
    MotorPid motorB = MotorPid(CorePins::MotorPHB, CorePins::MotorENB, &AbsEncorders, 1);
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
        pinMode(CorePins::Buzzer, OUTPUT);
        pinMode(CorePins::ENG_SW, INPUT);
        pinMode(CorePins::SW, INPUT);
        pinMode(CorePins::Debug_LED, OUTPUT);
        pinMode(CorePins::Alive_LED, OUTPUT);

        analogWriteFreq(10000);  // 必須
        analogWriteRange(65535); // 必須
        motorA.setSaturation(65535, -65535);
        motorA.begin();
        motorB.setSaturation(65535, -65535);
        motorB.begin();
    }
};

TickTwo alive(
    []() {
        digitalWrite(CorePins::Alive_LED, !digitalRead(CorePins::Alive_LED));
    },
    500, 0, MILLIS);

void login1() {
    tone(CorePins::Buzzer, 3900);
    delay(20);
    tone(CorePins::Buzzer, 3000);
    delay(50);
    tone(CorePins::Buzzer, 6000);
    delay(30);

    tone(CorePins::Buzzer, 10000);
    delay(20);
    tone(CorePins::Buzzer, 4200);
    delay(50);
    noTone(CorePins::Buzzer);
}

void login2() {
    tone(CorePins::Buzzer, 9000);
    delay(100);
    tone(CorePins::Buzzer, 7200);
    delay(100);
    tone(CorePins::Buzzer, 1200);
    delay(100);
    tone(CorePins::Buzzer, 8000);
    delay(100);
    noTone(CorePins::Buzzer);
}

void logout1() {
    tone(CorePins::Buzzer, 6000);
    delay(20);
    tone(CorePins::Buzzer, 11000);
    delay(50);
    tone(CorePins::Buzzer, 2000);
    delay(50);
    tone(CorePins::Buzzer, 6000);
    delay(30);
    tone(CorePins::Buzzer, 4000);
    delay(50);
    noTone(CorePins::Buzzer);
}

void setup1() {
    // login
    login1();
    delay(200);
    login2();
    alive.start();
}

void loop1() {
    alive.update();
}
#endif