#ifndef __ControlMotorMode__
#define __ControlMotorMode__

#include <./Modes/Mode.hpp>

class ControlMotorMode : public Mode, Robot {
    using Robot::Robot;

  public:
    ControlMotorMode(char letter, const char name[]) : Mode(letter, name) {}

    void init() {
        deviceBegin();
    }

    void before() {
        Serial.printf("before %s\n", getModeName());

        startAngle1 = AbsEncorders.readRadian(0);
        startAngle2 = AbsEncorders.readRadian(1);
        currentAngle1 = 0;
        currentAngle2 = 0;
        round1 = false;
        round2 = false;
        roundCnt1 = 0;
        roundCnt2 = 0;
        velPrev1 = 0;
        velPrev2 = 0;

        // set PD gain
        angleKp[0] = 0;
        angleKp[1] = 0;
        angleKd[0] = 0;
        angleKd[1] = 0;
        velKp[0] = 0;
        velKp[1] = 0;
        velKd[0] = 0;
        velKd[1] = 0;

        for (size_t i = 0; i < 50; i++) {
            angle1 = AbsEncorders.readRadian(0); // 0番目のエンコーダの角度を取得
            angle2 = AbsEncorders.readRadian(1); // 1番目のエンコーダの角度を取得
        }
        interval.reset();
    }

    void loop() {
        Serial.printf("loop:%d\n waiting for send 'M' ", getModeLetter());

        // initialize
        analogWrite(CorePins::MotorA, 0);
        analogWrite(CorePins::MotorB, 0);

        highVel = HALF_PI; // rad/s
        lowVel = HALF_PI / 2;

        // 5ms ごとにセンサの値を読み，角速度を計算し，シリアルモニタに出力する
        // 5ms ごとにモータに入力を与える
        if (interval.read_us() >= Ts * 1000000) {
            interval.reset();

            // いろいろ更新
            velPrev1 = vel1;
            velPrev2 = vel2;
            angle1 = AbsEncorders.readRadian(0); // 0番目のエンコーダの角度を取得
            angle2 = AbsEncorders.readRadian(1); // 1番目のエンコーダの角度を取得
            vel1 = -AbsEncorders.getVelocity(0); // 0番目のエンコーダの角速度を取得[rad/s]
            vel2 = -AbsEncorders.getVelocity(1); // 1番目のエンコーダの角速度を取得[rad/s]
            acc1 = (vel1 - velPrev1) / Ts;
            acc2 = (vel2 - velPrev2) / Ts;

            // ここに制御則とか書く
            if (vel1 < lowVel || vel2 < lowVel) { // 目標の低角速度まで加速
                analogWrite(CorePins::MotorA, voltToDuty(12));
                analogWrite(CorePins::MotorB, voltToDuty(12));
            } else { // 運転できるようになってからの制御
            }

            // input voltage to motor (0~12V)
            analogWrite(CorePins::MotorA, voltToDuty(12)); // ここ12の値を変更
            analogWrite(CorePins::MotorB, voltToDuty(12));
        }

        // end process
        analogWrite(CorePins::MotorA, 0);
        analogWrite(CorePins::MotorB, 0);
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
    float angle1, angle2;               // 0 ~ 2pi rad
    float startAngle1, startAngle2;     // 0 ~ 2pi rad
    float currentAngle1, currentAngle2; // 0 ~ float_max rad : 累積弧度
    float vel1, vel2;                   // rad/s
    float acc1, acc2;                   // rad/s/s
    float velPrev1, velPrev2;
    bool round1, round2;          // １周したことを判別
    uint8_t roundCnt1, roundCnt2; // 何周したかをカウント

    // 制御系
    float highVel, lowVel;
    float goalVel1, goalVel2;
    float volt1, volt2; // モータに入力する電圧
    float angleKp[2], angleKd[2], velKp[2], velKd[2];

    uint16_t voltToDuty(float volt) {
        return volt * 65535 / 12;
    }
};

extern ControlMotorMode controlmotor;

#endif