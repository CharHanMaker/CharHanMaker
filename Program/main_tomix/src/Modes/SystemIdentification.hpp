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
        Serial.println("Time[ms], Volt[V], Angle1[rad], AngleVel1[rad/s], Angle2[rad], AngleVel2[rad/s]");
        // initialize
        motorA.stop();
        motorB.stop();
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
        Serial.println("Time[ms], Volt[V], Angle1[rad], AngleVel1[rad/s], Angle2[rad], AngleVel2[rad/s]");

        while (!isDone) {
            if (interval.read_ms() >= 5) { // 周期依存 Ts = 0.005のとき5ms
                interval.reset();
                sysIdentification();
            }
        }

        motorA.stop();
        motorB.stop();
        Serial.printf("end...Send T to continue\n");
        delay(10000);
    }

    void after() {
        Serial.printf("after:%d\n", getModeLetter());
    }

    void sysIdentification() {
        float encCnt; // エンコーダのカウント
        float omega;  // 角速度[rad/s]
        float omega1;
        float theta;
        float theta1;
        int u_pwm; // PWMのDuty比

        // NOTE: なぜなのか！？0番目のエンコーダの角度を取得しないと1番目のエンコーダの角度が取得できないｗｗｗｗｗｗｗｗｗｗｗｗ
        theta = AbsEncorders.readRadian(0);
        theta1 = AbsEncorders.readRadian(1);
        omega = AbsEncorders.getVelocity(0);
        omega1 = AbsEncorders.getVelocity(1);

        // Time[ms], Volt[V], Angle1[rad], AngleVel1[rad/s], Angle2[rad], AngleVel2[rad/s]
        Serial.printf("%d, %.3f, %.3f, %.3f, %.3f, %.3f\n", Time.read_ms(), u * 12, theta, omega, theta1, omega1);

        t += Ts; // 時間の更新

        u = sin(2 * PI * f * t + ofset); // 入力の計算
        u_pwm = int(u * 65535 / 2);      // 入力をPWMのDuty比に変換

        motorA.runOpenloop(u_pwm, true);
        motorB.runOpenloop(u_pwm, true);

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
            motorA.stop();
            motorB.stop();
            isDone = true;
        }
    }

  private:
    timer interval;
    float Ts = 0.005; // 周期[s]

    bool isDone = false;

    float u = 0;           // 入力
    float t = 0;           // 時間
    float f = 0.1;         // 入力の周波数
    const float ofset = 0; //-90 * DEG_TO_RAD;
};

extern SystemIdentificationMode systemidentificationMode;

#endif