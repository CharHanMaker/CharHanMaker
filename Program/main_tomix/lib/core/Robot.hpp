#ifndef __Robot__
#define __Robot__

#include "core.hpp"
#include "MyMath.hpp"
#include "TickTwo.h"
#include "Motor.hpp"
#include <Wire.h>
#include <timer.h>
#include <EEPROM.h>
#include "LPF.hpp"
#include "MultipleAS5600.hpp"
#include <MotorPID.hpp>

typedef struct {
    uint16_t angleLeft;    //[deg]
    uint16_t angleRight;   //[deg]
    float angulerVelLeft;  //[rad/s]
    float angulerVelRight; //[rad/s]
} sensors_t;

typedef union {
    uint16_t dataInt; // main value
    uint8_t bytes[2]; // byte array
} byteSplitterInt16;

void readEncoderZeroPos(uint16_t &rawValue0, uint16_t &rawValue1);
void writeEncoderZeroPos(uint16_t rawValue0, uint16_t rawValue1);

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

        // EEPROM
        EEPROM.begin(512);
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

        for (size_t i = 0; i < 5; i++) {
            // 空読み
            AbsEncorders.readRawValue(0);
            AbsEncorders.readRawValue(1);
        }
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

void readEncoderZeroPos(uint16_t &rawValue0, uint16_t &rawValue1) { // EEPROM
    byteSplitterInt16 dataFrame[2];
    for (size_t address = 0; address < 2; address++) {
        dataFrame[address].bytes[0] = EEPROM.read(address * 2);     // LSB
        dataFrame[address].bytes[1] = EEPROM.read(address * 2 + 1); // MSB
        Serial.printf("%d: %d ,%d → %d\n", address, dataFrame[address].bytes[0], dataFrame[address].bytes[1], dataFrame[address].dataInt);
    }
    rawValue0 = dataFrame[0].dataInt;
    rawValue1 = dataFrame[1].dataInt;
}

void writeEncoderZeroPos(uint16_t rawValue0, uint16_t rawValue1) { // EEPROM
    byteSplitterInt16 dataFrame[2];
    dataFrame[0].dataInt = rawValue0;
    dataFrame[1].dataInt = rawValue1;
    for (size_t address = 0; address < 2; address++) {
        EEPROM.write(address * 2, dataFrame[address].bytes[0]); // LSB
        delay(200);
        EEPROM.write(address * 2 + 1, dataFrame[address].bytes[1]); // MSB
        delay(200);
        Serial.printf("%d: %d → %d ,%d \n", address, dataFrame[address].dataInt, dataFrame[address].bytes[0], dataFrame[address].bytes[1]);
        if (EEPROM.commit()) {
            Serial.println("EEPROM successfully committed");
            login2();
        } else {
            Serial.println("ERROR! EEPROM commit failed");
            logout1();
        }
    }
    delay(2000);
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