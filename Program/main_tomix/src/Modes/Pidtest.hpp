#ifndef __Pidtest__
#define __Pidtest__

#include <./Modes/Mode.hpp>

class Pidtest : public Mode, Robot {
    using Robot::Robot;

  public:
    Pidtest(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("before:%d\n", getModeLetter());
    }

    void loop() {
        analogWrite(CorePins::MotorA, 0);
        delay(100);
        Time.reset();
        interval.reset(); // 制御周期
        Kp = 1, Ki = 0.5, Kd = 1;

        float e_pre = 0;      // 微分用の差
        float integral_e = 0; // 積分用の差

        // ここでPIDに則ったモータの動きをする
        while (Time.read_ms() / 1000 < 10) { // 実行時間：10s
            if (interval.read_ms() >= 10) {  // 10msごとに実行
                float y0 = AbsEncorders.getVelocity(0);

                // 目標角速度を取得 ３周期に１回,部分的に速く回す
                float r0 = 0;
                if (Time.read_ms() % (3 * interval.read_ms()) < 2.8 * interval.read_ms())
                    r0 = 0.15; // rad/s
                else
                    r0 = 0.5;

                float e0 = r0 - y0;
                float de = (e0 - e_pre) / interval.read_ms();                    // 誤差の微分を近似計算
                integral_e = integral_e + (e0 + e_pre) * interval.read_ms() / 2; // 誤差の積分を近似計算
                float u = Kp * e0 + Ki * integral_e + Kd * de;                   // 入力電圧

                if (u > 12)
                    u = 12;
                else if (u < 0)
                    u = 0;

                // モータドライバへの出力
                analogWrite(CorePins::MotorA, u);

                interval.reset();
                e_pre = e0;
            }
        }

        analogWrite(CorePins::MotorA, 0);
        Serial.printf("end...Send T to continue\n");
        delay(10000);
    }

    void after() {
        Serial.printf("after:%d\n", getModeLetter());
    }

  private:
    timer interval;
    float pre_w;
    float Kp = 1, Ki = 0.5, Kd = 1;
};

extern Pidtest pidtest;

#endif