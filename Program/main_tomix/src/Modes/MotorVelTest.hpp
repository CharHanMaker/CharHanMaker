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

        motorA.setSaturation(65535, -65535);

        velPID.setLimit(-12, 12);
        velPID.reset();

        targetVel = 0;
        currentVel = 0;
        previousVel = 0;

        Serial.setTimeout(5);
        isLoop = true;
        AbsEncorders.readDegree(0);
        AbsEncorders.readDegree(1);
        while (abs(AbsEncorders.readDegree(0)) > 0.1) {
            motorA.runOpenloop(voltToDuty(1), true);
        }
        Serial.printf("ZeroDeg0:%.2f\n", float(AbsEncorders.readDegree(0)));
    }

    void loop() {
        while (isLoop) {
            // 文字列受信
            if (Serial.available() > 0) {
                String str = Serial.readStringUntil('\n'); // 改行までの文字列を受信
                if (str == "stop") {                       // stopと入力されたらモータを止める
                    isLoop = false;                        // whileループを抜ける
                    motorA.stop();
                    motorB.stop();
                    logout1();
                } else if (str.startsWith("set")) {
                    str.remove(0, 3); // "set"の3文字部分を削除
                    targetVel = str.toFloat();
                    velPID.resetIntegral();
                }
            } else {
                AbsEncorders.read12BitValue(0);
                AbsEncorders.read12BitValue(1);
                currentVel = AbsEncorders.getVelocity(0); //[rad/s]
                error = -(targetVel - currentVel);        // これはmotorA.setCW()でなくせるはず
                velPID.appendError(error);
                velPID.compute();
                output = velPID.getPID();
                motorA.runOpenloop(voltToDuty(output), true);
                Serial.printf("targetVel:%.2f, currentVel:%.2f, error:%.2f, output:%.2f\n", targetVel, currentVel, error, output);
                // delay(3);
                // if (abs(error) < 0.1) velPID.reset();
            }
        }

        Serial.println("Send 'T' to reset");
        delay(1000);
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
        motorA.stop();
        motorB.stop();
    }

  private:
    const float Kp = 0.0;
    const float Ki = 1.0;
    const float Kd = 0.0;
    const float dt = 0.003; // 5ms
    float targetVel;
    float currentVel;
    float previousVel;
    float error;
    float output;
    PID velPID = PID(Kp, Ki, Kd, dt);

    bool isLoop = true;

    // functions
    int32_t voltToDuty(float volt) {
        return int32_t(volt * 65535 / 12);
    }
};

extern MotorVelTest motorVelTest;

#endif