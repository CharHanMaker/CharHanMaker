#ifndef __SynchronizeTest__
#define __SynchronizeTest__

#include <./Modes/Mode.hpp>

class SynchronizeTest : public Mode, Robot {
    using Robot::Robot;

  public:
    SynchronizeTest(char letter, const char name[]) : Mode(letter, name) {}

    void init() {
        deviceBegin();
    }

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

        highVel = 30 * PI / 180; // rad/s = 30[deg/s]
        lowVel = 10 * PI / 180;  // 5[deg/s]
        for (size_t i = 0; i < 2; i++) {
            vel[i].target = lowVel;
        }

        motorA.setSaturation(65535, -65535);
        motorB.setSaturation(65535, -65535);

        velPID[0].setLimit(-12, 12);
        velPID[0].reset();
        velPID[1].setLimit(-12, 12);
        velPID[1].reset();

        angPID[0].setLimit(-6, 6);
        angPID[0].reset();
        angPID[1].setLimit(-6, 6);
        angPID[1].reset();

        Serial.setTimeout(5);
        isLoop = true;

        AbsEncorders.readRadian(0);
        AbsEncorders.readRadian(1);

        turnMotorZeroPos();
    }

    void loop() {
        while (isLoop) {
            getSensors();
            resetVolt();

            // // 目標の定速へPID
            // for (size_t i = 0; i < 2; i++) {
            //     vel[i].error = -(vel[i].target - vel[i].current);
            //     velPID[i].appendError(vel[i].error);
            //     velPID[i].compute();

            //     volt[i] += velPID[i].getPID();
            // }

            // // 角度同期
            angleContinuous[0].target = angleContinuous[1].current;                              // 目標値
            angleContinuous[0].error = (angleContinuous[0].target - angleContinuous[0].current); // 偏差
            angPID[0].appendError(angleContinuous[0].error);
            angPID[0].compute();
            volt[0] += angPID[0].getPID();

            // モータへ電圧を入力
            volt[1] = 1;
            saturateVolt(2);
            motor[0]->runOpenloop(voltToDuty(volt[0]), true);
            motor[1]->runOpenloop(voltToDuty(volt[1]), true);

            // 結果を出力したい
            Serial.printf("currentAng1:%.2f, vel1:%.2f, volt1:%.2f, currentAng2:%.2f, vel2:%.2f, volt2:%.2f, error:%.2f\n", angleContinuous[0].current, vel[0].current, volt[0], angleContinuous[1].current, vel[1].current, volt[1], angleContinuous[0].error);

            // for (size_t i = 0; i < 2; i++) {
            //     Serial.printf("targetVel:%.2f, currentVel:%.2f, error:%.2f, output:%.2f\n", vel[i].target, vel[i].current, vel[i].error, vel[i].output);
            // }
        }

        motorA.stop();
        motorB.stop();

        Serial.println("Send 'T' to reset");
        delay(1000);
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

    void saturateVolt(float absRange) { // volt : -absRange ~ +absRange [V]
        absRange = abs(absRange);
        for (size_t i = 0; i < 2; i++) {
            volt[i] = ((volt[i]) < -absRange ? -absRange : ((volt[i]) > absRange ? absRange : (volt[i])));
            // motor[i]->runOpenloop(voltToDuty(volt[i]), true);
        }
    }

    void turnMotorZeroPos() {
        AbsEncorders.readDegree(0);
        AbsEncorders.readDegree(1);
        while (abs(AbsEncorders.readDegree(0)) > 0.1) {
            AbsEncorders.readDegree(0);
            AbsEncorders.readDegree(1);
            motorA.runOpenloop(voltToDuty(1), true);
        }
        motorA.stop();
        AbsEncorders.resetRotationCount(0);

        AbsEncorders.readDegree(0);
        AbsEncorders.readDegree(1);
        while (abs(AbsEncorders.readDegree(1)) > 0.1) {
            AbsEncorders.readDegree(0);
            AbsEncorders.readDegree(1);
            motorB.runOpenloop(voltToDuty(1), true);
        }
        motorB.stop();
        AbsEncorders.resetRotationCount(1);
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
        .Ki = 0.005,
        .Kd = 0.005,
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

    // functions
    int32_t voltToDuty(float volt) {
        return int32_t(volt * 65535 / 12);
    }

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