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
        pid.setLimit(0, 12); // 出力の最大値を設定
        pid.setGain(Kp, Ki, Kd);
    }
    void before() {
    }

    void loop() {
        // シリアルで速度を受信してそれをtagrgetにする

        float angle1, angle2;
        int time = 0;
        // time = Time.read_us();
        // Time.reset();                             //
        angle1 = AbsEncorders.readDegree(0);      // 0番目のエンコーダの角度を取得
        angle2 = AbsEncorders.readDegree(1);      // 1番目のエンコーダの角度を取得
        float vel = -AbsEncorders.getVelocity(0); // 1番目のエンコーダの角速度を取得[rad/s]

        // Serial.printf("%.2f, %.2f, vel:%.2f, time:%dus\n", angle1, angle2, vel, time);
        float error = target - vel; // 目標値と現在値の偏差を計算
                                    // pid.appendError(error);  // 偏差をPIDに追加
                                    // pid.compute();    p       // PIDの計算

        // pid
        I += error * dt;
        if (I > 12) I = 12;
        if (I < -12) I = -12;
        float output = error * Kp; //+ I * Ki;
        float out__ = output * 5461.25;
        // 何かしら出力をする(analogWriteなど..)
        analogWrite(CorePins::MotorA, out__);

        Serial.printf("targetVel:%.2f,vel:%.2f, out:%f,out__:%f,time:%dus\n", target, vel, output, I, time);
        delay(5);
    }

    void after() {
        I = 0;
        analogWrite(CorePins::MotorA, 0);
    }

  private:
    PID pid;
    const float Kp = 1;
    const float Ki = 0;
    const float Kd = 0.001;
    const float dt = 0.005;
    float target = 10;
    timer Time;
    float I;
};

extern MainMode mainMode;

#endif