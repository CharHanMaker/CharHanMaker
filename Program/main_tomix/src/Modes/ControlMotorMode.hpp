#ifndef __ControlMotorMode__
#define __ControlMotorMode__

#include <./Modes/Mode.hpp>
#include <MotorPID.hpp>

class ControlMotorMode : public Mode, Robot {
    using Robot::Robot;

  public:
    ControlMotorMode(char letter, const char name[]) : Mode(letter, name) {}

    void init() {
        deviceBegin();
    }

    void before() {
        Serial.printf("before %s\n", getModeName());

        for (size_t i = 0; i < 50; i++) {
            angle[0] = AbsEncorders.readRadian(0); // 0番目のエンコーダの角度を取得
            angle[1] = AbsEncorders.readRadian(1); // 1番目のエンコーダの角度を取得
        }

        for (uint8_t i = 0; i < 2; i++) {
            initialAngle[i] = AbsEncorders.setZero(i);
            continuousAngle[i] = 0;
            rotationCnt[i] = AbsEncorders.resetRotationCount(i);
            velPrev[i] = 0;

            goalAngle[i] = 0;
            goalVel[i] = 0;
            goalVelPrev[i] = 0;

            volt[i] = 0;
        }
        highVel = HALF_PI; // rad/s
        lowVel = HALF_PI / 2;

        continuousTime = 0; // ms
        cycleTime = 400;    // 0.4s
        integralTime = 0;

        interval.reset();
        continuousTimer.reset(); // [ms]
    }

    void loop() {
        // 5ms ごとにセンサの値を読み，角速度を計算し，シリアルモニタに出力する
        // 5ms ごとにモータに入力を与える
        if (interval.read_us() >= Ts * 1000000) {
            interval.reset();

            // いろいろ更新
            for (uint8_t i = 0; i < 2; i++) {
                velPrev[i] = vel[i];
                angle[i] = AbsEncorders.readRadian(i); // i番目のエンコーダの弧度を取得
                vel[i] = -AbsEncorders.getVelocity(i); // i番目のエンコーダの角速度を取得[rad/s] これのマイナスどうする？
                acc[i] = (vel[i] - velPrev[i]) / Ts;

                rotationCnt[i] = AbsEncorders.getRotationCount(i);                          // overflow対策どうする 片方3000回転でもう片方が30回転とかあり得る
                continuousAngle[i] = AbsEncorders.getContinuousRadian(i) - initialAngle[i]; // rotationCntでoverflowを対処するので、ここでは考慮しなくて良い
            }

            // 目標角速度を決める、
            integralTime = continuousTimer.read_ms(); // uint8_t と unsigned long だからerrorが生じるかも
            continuousTime += integralTime;
            if (continuousTime > 100 * cycleTime) continuousTime -= 99 * cycleTime;
            continuousTimer.reset();

            if (continuousTime % cycleTime <= 300) { // t < T*3/4
                for (uint8_t i = 0; i < 2; i++) {
                    goalVelPrev[i] = goalVel[i]; // 更新
                    goalVel[i] = lowVel;
                }
            } else {
                for (uint8_t i = 0; i < 2; i++) {
                    goalVelPrev[i] = goalVel[i]; // 更新
                    goalVel[i] = highVel;
                }
            }

            // 台形積分して目標角を出す overflow対策どうしよう
            for (uint8_t i = 0; i < 2; i++) {
                goalAngle[i] = (goalVelPrev[i] + goalVel[i]) * integralTime / 2; // uint8_tを割ってるので微誤差発生注意
            }

            // ここに制御則とか書く
            if (vel[0] < lowVel || vel[1] < lowVel) { // 目標の低角速度まで加速
                for (uint8_t i = 0; i < 2; i++)
                    volt[i]++;
            } else { // 実機が立ち上がってからの制御
                // 目標角速度と目標角をコントローラに入れてPID制御
                volt[0] = motorA.velControl(goalVel[0]) + motorA.angleControl(goalAngle[0]);
                volt[1] = motorB.velControl(goalVel[1]) + motorB.angleControl(goalAngle[1]);
            }
            // モータへ出力
            synchronizeMotors(); // 申し訳程度の同期
            motorA.runOpenloop(volt[0], true);
            motorB.runOpenloop(volt[1], true);
        }
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
    int32_t volt[2]; // モータに入力する電圧のpwm
    // float angleMaster, angleSlave; // master : A, slave : B で固定するから要らないかも
    unsigned long continuousTime, cycleTime; // 累計時間と周期[ms]
    uint8_t integralTime;

    // uint16_t voltToDuty(float volt) { // 要らない？
    //     return volt * 65535 / 12;
    // }

    // 常にAがmaster, Bがslave
    // angleの差に応じたpwm操作できると良いね
    void synchronizeMotors() {
        if (angle[0] < angle[1]) { // AがBに遅れてる
            volt[1]--;
        } else if (angle[0] > angle[1]) { // AがBより進んでる
            volt[1]++;
        }
    }
};

extern ControlMotorMode controlmotor;

#endif