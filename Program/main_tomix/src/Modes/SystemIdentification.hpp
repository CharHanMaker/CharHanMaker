#ifndef __SYSID__
#define __SYSID__

#include <./Modes/Mode.hpp>

class SystemIdentificationMode : public Mode, Robot {
    using Robot::Robot;

  public:
    SystemIdentificationMode(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("before:%d\n", getModeLetter());
    }

    void indentification() {
        float encCnt; // エンコーダのカウント
        float omega;  // 角速度[rad/s]
        uint16_t theta;
        int u_pwm;            // PWMのDuty比
        static float u = 0;   // 入力
        static float t = 0;   // 時間
        static float f = 0.1; // 入力の周波数

        theta = AbsEncorders.readDegree(0);
        omega = AbsEncorders.getVelocity(0);

        // ログデータの出力
        // Serial.print(u * 12, 4);
        // Serial.print(" : ");
        // Serial.println(omega, 4);

        Serial.printf("%d, %.4f, %d, %.4f\n", Time.read_ms(), u * 12, theta, omega);

        t += Ts; // 時間の更新

        u = 0.5 * sin(2 * PI * f * t) + 0.5; // 入力の計算
        u_pwm = int(u * 65535 / 2);          // 入力をPWMのDuty比に変換

        // モータドライバへの出力
        analogWrite(CorePins::MotorA, u_pwm);

        // 入力周波数の更新
        if (t > 1 / f) {
            t = 0;
            if (f < 1)
                f += 0.1;
            else if (f < 10)
                f += 1;
            else if (f < 100)
                f += 10;
        }

        // 入力周波数が100Hz以上になったら終了
        if (f >= 100) {
            analogWrite(CorePins::MotorA, 0);
            isDone = true;
        }
    }

    void loop() {
        // Serial.printf("loop:%d\n waiting for send 'M' ", getModeLetter());
        // delay(1000);

        Serial.println("Time[ms], Volt[V], Angle[rad], AngleVel[rad/s]");
        // initialize
        analogWrite(CorePins::MotorA, 0);
        delay(100);
        Time.reset();
        interval.reset();
        isDone = false;
        Serial.println("Time[ms], Volt[V], Angle[degree], AngleVel[rad/s]");
        while (!isDone) {
            if (interval.read_ms() >= 2) {
                interval.reset();
                indentification();
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
    float Ts = 0.001;

    bool isDone = false;
};

extern SystemIdentificationMode systemidentificationMode;

#endif