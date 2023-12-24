#ifndef __SynchronizeTest__
#define __SynchronizeTest__

#include <./Modes/Mode.hpp>

class SynchronizeTest : public Mode, Robot {
    using Robot::Robot;

  public:
    SynchronizeTest(char letter, const char name[]) : Mode(letter, name) {}

    // void init() { // 諸悪の根源
    //     deviceBegin();
    // }

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

        highVel = 30 * PI / 180; // rad/s = 30[deg/s]
        lowVel = 540 * PI / 180; // [deg/s]
        for (size_t i = 0; i < 2; i++) {
            // vel[i].target = lowVel;
            vel[i].current = 0;
        }

        motorA.setSaturation(65535, -65535);
        motorB.setSaturation(65535, -65535);

        voltageLimit = 12;
        velPID[0].setLimit(-voltageLimit, voltageLimit);
        velPID[0].reset();
        velPID[1].setLimit(-voltageLimit, voltageLimit);
        velPID[1].reset();

        angPID[0].setLimit(-voltageLimit, voltageLimit);
        angPID[0].reset();
        angPID[1].setLimit(-voltageLimit, voltageLimit);
        angPID[1].reset();

        AbsEncorders.setDirection(0, CCW);
        AbsEncorders.setDirection(1, CCW);

        Serial.setTimeout(5);
        isLoop = true;

        turnMotorZeroPos();
    }

    void loop() {
        while (isLoop) {
            if (Serial.available() > 0) {
                getSerialData();
            } else {
                getSensors();
                resetVolt();

                // volt[0] = 1;
                // volt[1] = 1;

                // 目標の定速へPID
                for (uint8_t i = 0; i < 2; i++) {
                    volt[i] += velCtrl(i, targetVel);
                }

                // 角度同期
                volt[0] += angleCtrl(0, angleContinuous[1].current);

                // saturate input voltage
                for (size_t i = 0; i < 2; i++) {
                    volt[i] = constrain(volt[i], -voltageLimit, voltageLimit);
                }

                // モータへ電圧を入力
                motor[0]->runOpenloop(voltToDuty(volt[0]), true);
                motor[1]->runOpenloop(voltToDuty(volt[1]), true);

                // 結果を出力したい
                Serial.printf("currentAng1:%.2f, vel1:%.2f, volt1:%.2f, currentAng2:%.2f, vel2:%.2f, volt2:%.2f, error:%.2f\n", angleContinuous[0].current, vel[0].current, volt[0], angleContinuous[1].current, vel[1].current, volt[1], angleContinuous[0].error);
            }
        }

        motorA.stop();
        motorB.stop();

        Serial.println("Send 'T' to reset");
        delay(1000);
    }

    void getSerialData() {
        String str = Serial.readStringUntil('\n'); // 改行までの文字列を受信
        if (str == "stop") {                       // stopと入力されたらモータを止める
            isLoop = false;                        // whileループを抜ける
            motorA.stop();
            motorB.stop();
            logout1();
        } else if (str.startsWith("set")) {
            str.remove(0, 3); // "set"の3文字部分を削除
            targetVel = str.toFloat();
            // vel[1].target = vel[0].target;
            // velPID.resetIntegral();
        }
    }

    void getSensors() {
        for (size_t i = 0; i < 2; i++) {
            // prev
            angle[i].prev = angle[i].current;
            angleContinuous[i].prev = angleContinuous[i].current;
            vel[i].prev = vel[i].current;

            // get Sensors
            angle[i].current = AbsEncorders.readRadian(i);
            angleContinuous[i].current = AbsEncorders.getContinuousRadian(i);
            vel[i].current = AbsEncorders.getVelocity(i); //[rad/s]
        }
    }

    void resetVolt() {
        for (size_t i = 0; i < 2; i++) {
            volt[i] = 0;
        }
    }

    float velCtrl(uint8_t id, float targetVel) {
        vel[id].target = targetVel;
        vel[id].error = -(vel[id].target - vel[id].current); // これはmotorA.setCW()でなくせるはず
        velPID[id].appendError(vel[id].error);
        velPID[id].compute();
        return vel[id].output = velPID[id].getPID();
    }

    float angleCtrl(uint8_t id, float targetAngle) {
        // angleContinuous[0].target = 0; //目標値
        angleContinuous[id].target = targetAngle;                                               // 目標値
        angleContinuous[id].error = (angleContinuous[id].target - angleContinuous[id].current); // 偏差
        angPID[id].appendError(angleContinuous[id].error);
        angPID[id].compute();
        return angleContinuous[id].output = angPID[id].getPID();
    }

    void saturateValue(float value, float absRange) { // volt : -absRange ~ +absRange [V]
        absRange = abs(absRange);
        value = constrain(value, -absRange, absRange);
    }

    void turnMotorZeroPos() {
        bool isOk = false;
        int16_t c1, c2 = 0;

        AbsEncorders.resetRotationCount(0);
        AbsEncorders.resetRotationCount(1);

        angPID[0].setLimit(-4, 4);
        angPID[0].reset();
        angPID[1].setLimit(-4, 4);
        angPID[1].reset();

        while (!isOk) {
            getSensors();
            angleCtrl(0, 0);
            angleCtrl(1, 0);
            if (abs(angleContinuous[0].current) < 0.15) {
                c1++;
                motorA.stop();
            } else {
                c1 = 0;
            }

            if (abs(angleContinuous[1].current) < 0.15) {
                c2++;
                motorB.stop();
            } else {
                c2 = 0;
            }

            if (c1 > 2000 && c2 > 2000) {
                isOk = true;
                break;
                AbsEncorders.resetRotationCount(0);
                AbsEncorders.resetRotationCount(1);
            }
            Serial.printf("C1:%d, C2:%d\n", c1, c2);
        }
        motorA.stop();
        motorB.stop();
        login1();

        AbsEncorders.resetRotationCount(0);
        AbsEncorders.resetRotationCount(1);

        angPID[0].setLimit(-voltageLimit, voltageLimit);
        angPID[0].reset();
        angPID[1].setLimit(-voltageLimit, voltageLimit);
        angPID[1].reset();
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
        motorA.stop();
        motorB.stop();
    }

  private:
    float highVel, lowVel;
    float volt[2];

    typedef struct {
        float Kp;
        float Ki;
        float Kd;
        float dt;
    } Gain;

    Gain velGain = {
        .Kp = 0.0,
        .Ki = 1.0,
        .Kd = 0.0,
        .dt = 0.003};

    Gain angGain = {
        .Kp = 7.0,
        .Ki = 0.1,
        .Kd = 0.01,
        .dt = 0.003};

    typedef struct {
        float target;
        float current;
        float prev;
        float error;
        float output;
    } ctrlValues;

    ctrlValues angle[2];
    ctrlValues angleContinuous[2];
    ctrlValues vel[2];

    MotorPid *motor[2] = {&motorA, &motorB};

    PID velPID[2] = {
        PID(velGain.Kp, velGain.Ki, velGain.Kd, velGain.dt),
        PID(velGain.Kp, velGain.Ki, velGain.Kd, velGain.dt)};
    PID angPID[2] = {
        PID(angGain.Kp, angGain.Ki, angGain.Kd, angGain.dt),
        PID(angGain.Kp, angGain.Ki, angGain.Kd, angGain.dt)};

    bool isLoop = true;
    float targetVel = 0;
    float voltageLimit = 0;

    // functions
    int32_t voltToDuty(float volt) {
        return int32_t(volt * 65535 / 12);
    }

    // 今のところ使ってない
    void synchronizeMotors() {
        if (abs(angle[0].target - angle[0].current) < abs(angle[1].target - angle[1].current)) { // motorAのほうが目標角に追従出来ている
            volt[1] += motorB.synchronizeControl(angle[0].current, angle[1].current);
        } else if (abs(angle[0].target - angle[0].current) > abs(angle[1].target - angle[1].current)) { // motorBのほうが目標角に追従出来ている
            volt[0] += motorA.synchronizeControl(angle[1].current, angle[0].current);
        }
    }
};

extern SynchronizeTest synchronizeTest;

#endif