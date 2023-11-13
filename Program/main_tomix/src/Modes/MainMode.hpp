#ifndef __MAIN__
#define __MAIN__

#include <./Modes/Mode.hpp>
#include <PID.hpp>

class MainMode : public Mode, Robot {
    using Robot::Robot;

  public:
    MainMode(char letter, const char name[]) : Mode(letter, name), pid(Kp, Ki, Kd, dt) {}
    int i = 270;
    void init() {
        deviceBegin();
        pid.setLimit(0, 65535); // 出力の最大値を設定
        pid.setGain(Kp, Ki, Kd);
    }
    void before() {
    }

    void loop() {
        // time = Time.read_us();
        // Time.reset(); //
        float angle1, angle2, time;
        angle1 = AbsEncorders.readDegree(0);     // 0番目のエンコーダの角度を取得
        angle2 = AbsEncorders.readDegree(1);     // 1番目のエンコーダの角度を取得
        float vel = AbsEncorders.getVelocity(0); // 1番目のエンコーダの角速度を取得[rad/s]

        // Serial.printf("%.2f, %.2f, vel:%.2f, time:%dus\n", angle1, angle2, vel, time);
        float error = 1.0 - vel; // 目標値と現在値の偏差を計算
        pid.appendError(error);  // 偏差をPIDに追加
        pid.compute();           // PIDの計算
        float output = pid.getPID();
        // 何かしら出力をする(analogWriteなど..)
        analogWrite(CorePins::MotorA, output);

        Serial.printf("%.2f, %.2f, vel:%.2f, time:%dus\n", angle1, angle2, vel, output, time);
    }

    void after() {
    }

  private:
    PID pid;
    const float Kp = 1;
    const float Ki = 1;
    const float Kd = 0;
    const float dt = 0.005;
};

extern MainMode mainMode;

#endif