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

        for (size_t i = 0; i < 50; i++) {          // これ要る？
            angle[0] = AbsEncorders.readRadian(0); // 0番目のエンコーダの角度を取得
            angle[1] = AbsEncorders.readRadian(1); // 1番目のエンコーダの角度を取得
        }

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
        highVel = 0.523598775; // rad/s = 30[deg/s]
        lowVel = 0.349065849;  // 20[deg/s]

        // PID gain
        motorA.setAnglePIDGain(0, 0, 0);
        motorA.setVelPIDGain(0, 0, 0);
        motorA.setSynchronizePIDGain(0, 0, 0);
        motorB.setAnglePIDGain(0, 0, 0);
        motorB.setVelPIDGain(0, 0, 0);
        motorB.setSynchronizePIDGain(0, 0, 0);

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
                vel[i] = -AbsEncorders.getVelocity(i); // i番目のエンコーダの角速度を取得[rad/s]
                acc[i] = (vel[i] - velPrev[i]) / Ts;

                goalVelPrev[i] = goalVel[i];

                rotationCnt[i] = AbsEncorders.getRotationCount(i);                          // overflow対策どうする 片方3000回転でもう片方が30回転とかあり得る
                continuousAngle[i] = AbsEncorders.getContinuousRadian(i) - initialAngle[i]; // rotationCntでoverflowを対処するので、ここでは考慮しなくて良い
            }

            // 目標角速度を決める、
            integralTime = continuousTimer.read_ms(); // uint8_t と unsigned long だからerrorが生じるかも
            continuousTime += integralTime;
            if (continuousTime > 100 * cycleTime) continuousTime -= 99 * cycleTime;
            continuousTimer.reset();

            if (vel[0] < lowVel || vel[1] < lowVel) { // 立ち上がり中の目標角速度 2s掛けてlowVelまで立ち上がる

                for (uint8_t i = 0; i < 2; i++) // ここ作業中
                    goalVel[i] += lowVel / 400; // lowVel / 2000ms * 5ms毎

            } else {                                     // 実機が立ち上がってからの目標角速度 要変更
                if (continuousTime % cycleTime <= 300) { // t <= T*3/4
                    for (uint8_t i = 0; i < 2; i++) {
                        goalVel[i] = lowVel;
                    }
                } else { // t > T*3/4
                    for (uint8_t i = 0; i < 2; i++) {
                        goalVel[i] = highVel;
                    }
                }
            }

            // 台形積分して目標角を出す overflow対策どうしよう
            for (uint8_t i = 0; i < 2; i++) {
                goalAngle[i] = (goalVelPrev[i] + goalVel[i]) * integralTime / 2; // uint8_tを割ってるので微誤差発生注意
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
            // Serial.println
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

extern ControlMotorMode controlmotor;

#endif