#ifndef __MotorVelTest__
#define __MotorVelTest__

#include <./Modes/Mode.hpp>

class MotorVelTest : public Mode, Robot {
    using Robot::Robot;

  public:
    MotorVelTest(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("loop %s\n", getModeName());
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
        login1();

        velPID.setLimit(-12, 12);
        velPID.reset();

        targetVel = 0;
        currentVel = 0;
        previousVel = 0;

        Serial.setTimeout(5);
    }

    void loop() {
        if (Serial.available() > 0) {
            targetVel = Serial.parseFloat();
        }
        AbsEncorders.read12BitValue(0);
        currentVel = AbsEncorders.getVelocity(0); //[rad/s]
        velPID.appendError(targetVel - currentVel);
        velPID.compute();
        output = velPID.getPID();
        motorA.runOpenloop(voltToDuty(output), 0);
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
        motorA.stop();
        motorB.stop();
    }

  private:
    const float Kp = 0.1;
    const float Ki = 0.0;
    const float Kd = 0.0;
    const float dt = 0.005; // 5ms
    float targetVel;
    float currentVel;
    float previousVel;
    float output;
    PID velPID = PID(Kp, Ki, Kd, dt);

    // functions
    uint16_t voltToDuty(float volt) {
        return volt * 65535 / 12;
    }
};

extern MotorVelTest motorVelTest;

#endif