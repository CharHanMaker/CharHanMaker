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

        velPrev1 = 0;
        velPrev2 = 0;
        for (size_t i = 0; i < 50; i++) {
            angle1 = AbsEncorders.readDegree(0); // 0番目のエンコーダの角度を取得
            angle2 = AbsEncorders.readDegree(1); // 1番目のエンコーダの角度を取得
        }
        interval.reset();
    }

    void loop() {
        Serial.printf("loop:%d\n waiting for send 'M' ", getModeLetter());

        // initialize
        analogWrite(CorePins::MotorA, 0);
        analogWrite(CorePins::MotorB, 0);

        // 5ms ごとにセンサの値を読み，角速度を計算し，シリアルモニタに出力する
        // 5ms ごとにモータに入力を与える
        if (interval.read_us() >= Ts * 1000000) {
            interval.reset();
            velPrev1 = vel1; // velPrevを更新
            velPrev2 = vel2;

            angle1 = AbsEncorders.readDegree(0); // 0番目のエンコーダの角度を取得
            angle2 = AbsEncorders.readDegree(1); // 1番目のエンコーダの角度を取得
            vel1 = -AbsEncorders.getVelocity(0); // 0番目のエンコーダの角速度を取得[rad/s]
            vel2 = -AbsEncorders.getVelocity(1); // 1番目のエンコーダの角速度を取得[rad/s]
            acc1 = (vel1 - velPrev1) / Ts;
            acc2 = (vel2 - velPrev2) / Ts;

            // ここに制御則とか書く

            // input voltage to motor
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
    float angle1, angle2;
    float vel1, vel2;
    float acc1, acc2;
    float velPrev1, velPrev2;

    // 制御系
    float volt1, volt2; // モータに入力する電圧

    uint16_t voltToDuty(float volt) {
        return volt * 65535 / 12;
    }
};

extern ControlMotorMode controlmotor;

#endif