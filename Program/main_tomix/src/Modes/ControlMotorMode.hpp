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
            startAngle[i] = angle[i];
            currentAngle[i] = 0;
            roundCnt[i] = 0;
            velPrev[i] = 0;

            goalAngle[i] = 0;
            goalVel[i] = 0;
            goalVelPrev[i] = 0;

            volt[i] = 0;
        }
        highVel = HALF_PI; // rad/s
        lowVel = HALF_PI / 2;

        currentTime = 0; // ms
        cycleTime = 400; // 0.4s

        interval.reset();
        currentTimer.reset(); // [ms]
        integralTime = 0;
    }

    void loop() {
        Serial.printf("loop:%d\n waiting for send 'M' ", getModeLetter());

        // initialize
        analogWrite(CorePins::MotorENA, 0);
        analogWrite(CorePins::MotorENB, 0);

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

                // モータが１周回転したことを確認 velPrevは間違い
                // if (0 <= angle[i] && angle[i] < PI && PI < velPrev[i] && velPrev[i] < TWO_PI)
                //     roundCnt[i]++;

                // currentAngle[i] = (float)roundCnt[i] * TWO_PI + angle[i] - startAngle[i];
            }

            // 目標角速度を決める、
            integralTime = currentTimer.read_ms(); // uint8_t と unsigned long だからerrorが生じるかも
            currentTime += integralTime;
            if (currentTime > 100 * cycleTime) currentTime -= 99 * cycleTime;
            currentTimer.reset();

            if (currentTime % cycleTime <= 300) { // t < T*3/4
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

            // 台形積分して目標角を出す
            for (uint8_t i = 0; i < 2; i++) {
                goalAngle[i] = (goalVelPrev[i] + goalVel[i]) * integralTime / 2; // uint8_tを割ってるので微誤差発生
            }

            // ここに制御則とか書く
            if (vel[0] < lowVel || vel[1] < lowVel) { // 目標の低角速度まで加速
                for (uint8_t i = 0; i < 2; i++)
                    volt[i]++;

                synchronizeMotors();
                motorA.runOpenloop(volt[0]);
                motorB.runOpenloop(volt[1]);
            } else { // 運転できるようになってからの制御
                // 目標角速度と目標角をコントローラに入れてPID制御
                volt[0] = motorA.velControl(goalVel[0]) + motorA.angleControl(goalAngle[0]);
                volt[1] = motorB.velControl(goalVel[1]) + motorB.angleControl(goalAngle[1]);

                synchronizeMotors(); // 申し訳程度の同期
                motorA.runOpenloop(volt[0]);
                motorB.runOpenloop(volt[1]);
            }

            // input voltage to motor (0~12V)
            // 入力電圧のsaturationを書く
            analogWrite(CorePins::MotorENA, voltToDuty(12)); // ここ12の値を変更
            analogWrite(CorePins::MotorENB, voltToDuty(12));
        }

        // end process
        analogWrite(CorePins::MotorENA, 0);
        analogWrite(CorePins::MotorENB, 0);
        Serial.printf("end...Send T to continue\n");
        delay(10000);
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
    }

  private:
    timer interval;
    const float Ts = 0.005; // 周期[s]
    timer currentTimer;     // [ms]

    // エンコーダ系
    float angle[2];        // 0 ~ 2pi rad
    float startAngle[2];   // 0 ~ 2pi rad
    float currentAngle[2]; // 0 ~ float_max rad : 累積弧度
    float vel[2];          // rad/s
    float acc[2];          // rad/s/s
    float velPrev[2];
    uint8_t roundCnt[2]; // 何周したかをカウント

    // 制御系
    float highVel, lowVel;
    float goalAngle[2];
    float goalVel[2];
    float goalVelPrev[2];
    int32_t volt[2]; // モータに入力する電圧のpwm
    // float angleMaster, angleSlave; // master : A, slave : B で固定するから要らないかも
    unsigned long currentTime, cycleTime; // 累計時間と周期[ms]
    uint8_t integralTime;

    uint16_t voltToDuty(float volt) {
        return volt * 65535 / 12;
    }

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