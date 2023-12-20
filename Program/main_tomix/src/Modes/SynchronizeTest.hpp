#ifndef __SynchronizeTest__
#define __SynchronizeTest__

#include <./Modes/Mode.hpp>
#include <MotorPID.hpp>

class SynchronizeTest : public Mode, Robot {
    using Robot::Robot;

  public:
    SynchronizeTest(char letter, const char name[]) : Mode(letter, name) {}

    void init() {
        deviceBegin();
    }

    void before() {
        Serial.printf("before %s\n", getModeName());

        for (uint8_t i = 0; i < 2; i++) {
            initialAngle[i] = AbsEncorders.setZero(i);
            continuousAngle[i] = 0;
            AbsEncorders.resetRotationCount(i);
            rotationCnt[i] = AbsEncorders.getRotationCount(i);
            velPrev[i] = 0;
            vel[i] = 0;

            goalAngle[i] = 0;
            goalVel[i] = 0;
            goalVelPrev[i] = 0;

            volt[i] = 0;
        }

        motorA.setSaturation(65535, -65535);
        motorB.setSaturation(65535, -65535);

        highVel = 30 * PI / 180; // rad/s = 30[deg/s]
        lowVel = 10 * PI / 180;  // 20[deg/s]

        // PID gain
        motorA.setAnglePIDGain(0, 0, 0);
        motorA.setVelPIDGain(2, 20, 0);
        motorA.setSynchronizePIDGain(1, 5, 0);
        motorB.setAnglePIDGain(0, 0, 0);
        motorB.setVelPIDGain(2, 20, 0);
        motorB.setSynchronizePIDGain(1, 5, 0);

        isLoop = true;
        // while (abs(AbsEncorders.readDegree(0)) > 0.1) { // 初期位置まで回転 メモリから初期角度を読み込んだとき使う
        //     motorA.runOpenloop(voltToDuty(1), true);
        // }
        // while (abs(AbsEncorders.readDegree(1)) > 0.1) {
        //     motorB.runOpenloop(voltToDuty(1), true);
        // }

        interval.reset();
        continuousTimer.reset(); // [ms]
    }

    void loop() {
        while (isLoop) {
            Serial.printf("1\n");
            for (uint8_t i = 0; i < 2; i++) {
                AbsEncorders.read12BitValue(i);
                vel[i] = -AbsEncorders.getVelocity(i);
            }

            // モータへ出力
            motorA.runOpenloop(voltToDuty(volt[0]), true);
            motorB.runOpenloop(voltToDuty(volt[1]), true);

            // 結果を出力したい
            Serial.printf("goalVel:%.2f, currentVel:%.2f, volt:%.2f, gap:%.2f\n", goalVel[0], vel[0], volt[0], angle[0] - angle[1]);

            delay(15);
        }

        motorA.stop();
        motorB.stop();

        Serial.println("Send 'T' to reset");
        delay(1000);
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
        motorA.stop();
        motorB.stop();
    }

  private:
    timer interval;
    const float Ts = 0.005; // 周期[s]
    timer continuousTimer;  // [ms]

    // エンコーダ系
    float angle[2];           // 0 ~ 2pi rad
    float initialAngle[2];    // 0 ~ 2pi rad
    float continuousAngle[2]; // 0 ~ float_max rad : 累計弧度
    float vel[2];             // rad/s
    float acc[2];             // rad/s/s
    float velPrev[2];
    int16_t rotationCnt[2]; // 何周したかをカウント

    // 制御系
    float highVel, lowVel;
    float goalAngle[2];
    float goalVel[2];
    float goalVelPrev[2];
    float volt[2]; // モータに入力する電圧のpwm
    // float angleMaster, angleSlave; // master : A, slave : B で固定するから要らないかも
    unsigned long continuousTime, cycleTime; // 累計時間と周期[ms]
    uint8_t integralTime;

    bool isLoop = true;

    uint16_t voltToDuty(float volt) {
        return volt * 65535 / 12;
    }

    // 常にAがmaster, Bがslave
    // angleの差に応じたpwm操作できると良いね
    void synchronizeMotors() {
        // if (angle[0] < angle[1]) { // AがBに遅れてる
        //     volt[1] -= 15;
        // } else if (angle[0] > angle[1]) { // AがBより進んでる
        //     volt[1] += 15;
        // }

        if (abs(goalAngle[0] - angle[0]) < abs(goalAngle[1] - angle[1])) { // motorAのほうが目標角に追従出来ている
            volt[1] += motorB.synchronizeControl(angle[0], angle[1]);
        } else if (abs(goalAngle[0] - angle[0]) > abs(goalAngle[1] - angle[1])) { // motorBのほうが目標角に追従出来ている
            volt[0] += motorA.synchronizeControl(angle[1], angle[0]);
        }
    }
};

extern SynchronizeTest synchronizeTest;

#endif