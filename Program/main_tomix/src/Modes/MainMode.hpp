#ifndef __MAIN__
#define __MAIN__

#include <./Modes/Mode.hpp>

class MainMode : public Mode, Robot {
    using Robot::Robot;

  public:
    MainMode(char letter, const char name[]) : Mode(letter, name) {}
    void init() {
        deviceBegin();
        // for (size_t i = 0; i < 10; i++) {
        //     AbsEncorders.setZero(0);
        //     AbsEncorders.setZero(1);
        // }
    }

    void before() {
        /*　--------------------EEPROMからエンコーダの初期角度の読み込み-------------------- */
        // これをbeforeに書かないと初期角度をセットしないままで位置制御始まるので危ないよお
        Serial.println("===========================================");
        Serial.println("Read from EEPROM");
        uint16_t rawValue0, rawValue1;
        readEncoderZeroPos(rawValue0, rawValue1); // EEPROMから初期角度値の読み出し
        Serial.printf("ZeroDeg0:%.2f, ZeroDeg1:%.2f\n", float(rawValue0 * BIT_12_TO_DEGREE), float(rawValue1 * BIT_12_TO_DEGREE));
        AbsEncorders.setZero(0, rawValue0); // EEPROMに保存されていてたエンコーダの初期角度を使用する
        AbsEncorders.setZero(1, rawValue1); // EEPROMに保存されていてたエンコーダの初期角度を使用する
        Serial.println("===========================================");
        // ここまで
        /*　--------------------EEPROMからエンコーダの初期角度の読み込み-------------------- */

        i = 0;
        login1();
    }

    void loop() {
        // // モータを回すプログラムです．
        int run = 65535 * MyMath::sinDeg(i);
        motorA.runOpenloop(run, true); // これで無理やりモータを回せます．
        motorB.runOpenloop(run, true); // これで無理やりモータを回せます．
        delay(15);
        Serial.println(AbsEncorders.readDegree(0));
        i++;
    }

    void after() {
        motorA.stop();
        motorB.stop();
    }

  private:
    int i = 0;
};

#endif