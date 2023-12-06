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
        }
        highVel = HALF_PI; // rad/s
        lowVel = HALF_PI / 2;

        interval.reset();
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

                // モータが１周回転したことを確認
                if (0 <= angle[i] && angle[i] < PI && PI < velPrev[i] && velPrev[i] < TWO_PI)
                    roundCnt[i]++;

                currentAngle[i] = (float)roundCnt[i] * TWO_PI + angle[i] - startAngle[i];
            }

            // ここに制御則とか書く
            if (vel[0] < lowVel || vel[1] < lowVel) { // 目標の低角速度まで加速
                analogWrite(CorePins::MotorENA, voltToDuty(12));
                analogWrite(CorePins::MotorENB, voltToDuty(12));
            } else { // 運転できるようになってからの制御
                // 目標角速度を決める、
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
    float volt[2]; // モータに入力する電圧
    float angleKp[2], angleKd[2], velKp[2], velKd[2];
    float angleMaster, angleSlave;

    uint16_t voltToDuty(float volt) {
        return volt * 65535 / 12;
    }
};

extern ControlMotorMode controlmotor;

#endif