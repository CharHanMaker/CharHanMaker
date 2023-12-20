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
        motorA.setSynchronizePIDGain(5, 3, 0);
        motorB.setAnglePIDGain(0, 0, 0);
        motorB.setVelPIDGain(2, 20, 0);
        motorB.setSynchronizePIDGain(5, 3, 0);

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
            if (interval.read_us() >= Ts * 1000000) { // 5msごとに実行
                interval.reset();

                // いろいろ更新
                for (uint8_t i = 0; i < 2; i++) {
                    velPrev[i] = vel[i];
                    angle[i] = AbsEncorders.readRadian(i); // i番目のエンコーダの弧度を取得
                    vel[i] = -AbsEncorders.getVelocity(i); // i番目のエンコーダの角速度を取得[rad/s]
                    acc[i] = (vel[i] - velPrev[i]) / Ts;

                    goalVelPrev[i] = goalVel[i];

                    rotationCnt[i] = AbsEncorders.getRotationCount(i);                          // overflow対策どうする 片方3000回転でもう片方が30回転とかあり得る
                    continuousAngle[i] = AbsEncorders.getContinuousRadian(i) - initialAngle[i]; // rotationCntでoverflowを対処するので、ここでは考慮しなくて良い
                }

                // 目標角速度を決める、
                integralTime = continuousTimer.read_ms(); // uint8_t と unsigned long だからerrorが生じるかも
                continuousTime += integralTime;
                if (continuousTime > 100 * cycleTime) continuousTime -= 99 * cycleTime; // 一応のoverflow対策（いらなそう）
                continuousTimer.reset();

                if (vel[0] < lowVel || vel[1] < lowVel) { // 立ち上がり中の目標角速度 2s掛けてlowVelまで立ち上がる

                    for (uint8_t i = 0; i < 2; i++) // ここ作業中
                        goalVel[i] += lowVel / 400; // lowVel / 2000ms * 5ms毎

                } else { // 実機が立ち上がってからの目標角速度 lowVelで定速
                    for (uint8_t i = 0; i < 2; i++)
                        goalVel[i] = lowVel;
                }

                // 台形積分して目標角を出す overflow対策どうしよう
                for (uint8_t i = 0; i < 2; i++) {
                    goalAngle[i] = (goalVelPrev[i] + goalVel[i]) * integralTime / 2; // uint8_tを割ってるので微誤差発生注意

                    if (goalAngle[i] >= __FLT_MAX__ / 10) // 目標角がoverflowに近づくとモータ停止
                        isLoop = false;
                }

                // 目標角速度と目標角をコントローラに入れてPID制御
                volt[0] = motorA.velControl(goalVel[0]) + motorA.angleControl(goalAngle[0]); // 角速度だけで制御
                volt[1] = motorB.velControl(goalVel[1]) + motorB.angleControl(goalAngle[1]);

                synchronizeMotors(); // PIDで同期
                for (uint8_t i = 0; i < 2; i++) {
                    constrain(volt[i], -12, 12); // -12 ~ 12 V に saturation
                }

                // モータへ出力
                motorA.runOpenloop(voltToDuty(volt[0]), true);
                motorB.runOpenloop(voltToDuty(volt[1]), true);

                // 結果を出力したい
                Serial.printf("goalVel:%.2f, currentVel:%.2f, volt:%.2f, gap:%.2f\n", goalVel[0], vel[0], volt[0], angle[0] - angle[1]);
            }
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