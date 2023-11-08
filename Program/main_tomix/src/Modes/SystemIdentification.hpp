#ifndef __SYSID__
#define __SYSID__

#include <./Modes/Mode.hpp>

class SystemIdentificationMode : public Mode, Robot {
    using Robot::Robot;

  public:
    SystemIdentificationMode(char letter, const char name[]) : Mode(letter, name) {}

    void init() {
        deviceBegin();
    }

    void before() {
        Serial.printf("before:%d\n", getModeLetter());
    }

    void loop() {
        Serial.printf("loop:%d\n waiting for send 'M' ", getModeLetter());
        // delay(1000);

        Serial.println("Time[ms], Volt[V], Angle[rad], AngleVel[rad/s]");
        // initialize
        analogWrite(CorePins::MotorA, 0);
        for (size_t i = 0; i < 30; i++) {
            AbsEncorders.readDegree(0);
            AbsEncorders.readDegree(1);
            delay(1);
        }
        delay(100);
        Time.reset();
        interval.reset();
        isDone = false;
        f = 0.1;
        Serial.println("Time[ms], Volt[V], Angle[degree], AngleVel[rad/s]");

        while (!isDone) {
            if (interval.read_ms() >= 5) { // 周期依存 Ts = 0.005のとき5ms
                interval.reset();
                sysIdentification();
            }
        }

        analogWrite(CorePins::MotorA, 0);
        Serial.printf("end...Send T to continue\n");
        delay(10000);
        // float angle1 = AbsEncorders.readDegree(0); // 0番目のエンコーダの角度を取得
        // float angle2 = AbsEncorders.readDegree(1); // 1番目のエンコーダの角度を取得
        // float vel = AbsEncorders.getVelocity(0);   // 1番目のエンコーダの角速度を取得[rad/s]
        // Serial.printf("%.2f, vel:%.2f, time:%dus\n", angle1, vel, time);
        // delay(10);
    }

    void after() {
        Serial.printf("after:%d\n", getModeLetter());
    }

    void sysIdentification() {
        float encCnt; // エンコーダのカウント
        float omega;  // 角速度[rad/s]
        float theta;
        int u_pwm; // PWMのDuty比

        // NOTE: なぜなのか！？0番目のエンコーダの角度を取得しないと1番目のエンコーダの角度が取得できないｗｗｗｗｗｗｗｗｗｗｗｗ
        theta = AbsEncorders.readDegree(0);
        AbsEncorders.readDegree(1);
        omega = AbsEncorders.getVelocity(0);

        Serial.printf("%d, %.3f, %.3f, %.3f\n", Time.read_ms(), u * 12, theta, omega);

        t += Ts; // 時間の更新

        u = 0.5 * sin(2 * PI * f * t + ofset) + 0.5; // 入力の計算
        u_pwm = int(u * 65535 / 2);                  // 入力をPWMのDuty比に変換

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

  private:
    timer interval;
    float Ts = 0.005; // 周期[s]

    bool isDone = false;

    float u = 0;   // 入力
    float t = 0;   // 時間
    float f = 0.1; // 入力の周波数
    const float ofset = -90 * DEG_TO_RAD;
};

extern SystemIdentificationMode systemidentificationMode;

#endif