#ifndef __SYSID2__
#define __SYSID2__

#include <./Modes/Mode.hpp>

class SystemIdentificationMode2 : public Mode, Robot {
    using Robot::Robot;

  public:
    SystemIdentificationMode2(char letter, const char name[]) : Mode(letter, name) {}

    void init() {
        deviceBegin();
    }

    void before() {
        Serial.printf("before %s\n", getModeName());
        velPrev1 = 0;
        velPrev2 = 0;
        for (size_t i = 0; i < 50; i++) {
            angle1 = AbsEncorders.readDegree(0); // 0番目のエンコーダの角度を取得
            angle2 = AbsEncorders.readDegree(1);
        }
        Serial.printf("time, angle1, vel1, acc1\n");
        finish.reset();
        interval.reset();
        // Serial.printf("time, angle2, vel2, acc2");
    }

    void loop() {
        // システム同定． ステップ入力を与えてモータの回転速度を取得したい．
        // 5ms ごとにセンサの値を読み，角速度を計算し，シリアルモニタに出力する
        // 5ms ごとにモータに入力を与える

        while (finish.read_ms() < 3000) {
            if (interval.read_us() >= Ts * 1000000) {
                interval.reset();
                velPrev1 = vel1;
                velPrev2 = vel2;

                angle1 = AbsEncorders.readDegree(0); // 0番目のエンコーダの角度を取得
                angle2 = AbsEncorders.readDegree(1); // 1番目のエンコーダの角度を取得
                vel1 = -AbsEncorders.getVelocity(0); // 1番目のエンコーダの角速度を取得[rad/s]
                vel2 = -AbsEncorders.getVelocity(0); // 1番目のエンコーダの角速度を取得[rad/s]
                acc1 = (vel1 - velPrev1) / Ts;
                acc2 = (vel2 - velPrev2) / Ts;

                Serial.printf("%d,%.2f, %.2f, %.2f\n", finish.read_us(), angle1, vel1, acc1);
                // Serial.printf("%d,%.2f, %.2f, %.2f\n", time, angle2, vel2, acc2);

                // モータに入力を与える
                analogWrite(CorePins::MotorA, voltToDuty(12));
            }
        }
        analogWrite(CorePins::MotorA, 0);
        Serial.printf("end...Send T to continue\n");
        delay(1000);
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
    }

  private:
    timer interval;
    timer finish;
    const float Ts = 0.005; // 周期[s]

    float angle1, angle2;
    float vel1, vel2;
    float acc1, acc2;
    float velPrev1, velPrev2;

    uint16_t voltToDuty(float volt) {
        return volt * 65535 / 12;
    }
};

extern SystemIdentificationMode2 systemidentificationMode2;

#endif