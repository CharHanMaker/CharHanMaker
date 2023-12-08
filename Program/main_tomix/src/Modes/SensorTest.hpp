#ifndef __SensorTestMode__
#define __SensorTestMode__

#include <./Modes/Mode.hpp>

class SensorTestMode : public Mode, Robot {
    using Robot::Robot;

  public:
    SensorTestMode(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        login1();
        checkType = '-';
        Serial.printf("loop %s\n", getModeName());
        Serial.println("\n\n\n");
        Serial.println("Choose What you want to check!");
        Serial.println(" - 0 : Encoder Raw Value");
        Serial.println(" - 1 : Angle[Deg]");
        Serial.println(" - 2 : Angle[Rad]");
        Serial.println(" - 3 : Endless Angle[Deg]");
        Serial.println(" - 4 : Endless Angle[Rad]");
        Serial.println(" - 5 : Velocity[rad/s]");
        Serial.println(" - 6 : ALL Value");
        Serial.println(" - 7 : check Saved Zero Position");
        Serial.println(" - 8 : Save Zero Position NOW");
        do {
            while (Serial.available() == 0) {
                Serial.print(".");
                delay(500);
            }
            checkType = Serial.read();
            if (!(checkType >= '0' && checkType <= '8')) {
                Serial.print(checkType);
                Serial.println(": resend...!!");
            }
        } while (checkType == '-');
        login1();
    }

    void loop() {
        if (checkType >= '0' && checkType <= '6') {
            if (checkType >= '3' && checkType <= '5') {
                AbsEncorders.read12BitValue(0);
                AbsEncorders.read12BitValue(1);
            }
            if (checkType == '0' || checkType == '6') {
                Serial.printf("Val0:%d\tVal1:%d\t", AbsEncorders.readRawValue(0), AbsEncorders.readRawValue(1));
            }
            if (checkType == '1' || checkType == '6') {
                Serial.printf("Deg0:%.2f\tDeg1:%.2f\t", AbsEncorders.readDegree(0), AbsEncorders.readDegree(1));
            }
            if (checkType == '2' || checkType == '6') {
                Serial.printf("Rad0:%.2f\tRad1:%.2f\t", AbsEncorders.readRadian(0), AbsEncorders.readRadian(1));
            }
            if (checkType == '3' || checkType == '6') {
                Serial.printf("CDeg0:%.2f\tCDeg1:%.2f\t", AbsEncorders.getContinuousDegree(0), AbsEncorders.getContinuousDegree(1));
            }
            if (checkType == '4' || checkType == '6') {
                Serial.printf("CRad0:%.2f\tCRad1:%.2f\t", AbsEncorders.getContinuousRadian(0), AbsEncorders.getContinuousRadian(1));
            }
            if (checkType == '5' || checkType == '6') {
                Serial.printf("Vel0:%.2f\tVel1:%.2f\t", AbsEncorders.getVelocity(0), AbsEncorders.getVelocity(1));
            }
            Serial.println();
        } else if (checkType <= '7' && checkType <= '8') {
            // EEPROMの設定
            Serial.println("EEPROM");
            delay(1000);
        } else {
            Serial.println("Send 'T' to reset");
            delay(1000);
        }
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
        motorA.stop();
        motorB.stop();
    }

  private:
    char checkType;
};

extern SensorTestMode sensorTestMode;

#endif