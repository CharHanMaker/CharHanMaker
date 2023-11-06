#ifndef __MAIN__
#define __MAIN__

#include <./Modes/Mode.hpp>

class MainMode : public Mode, Robot {
    using Robot::Robot;

  public:
    MainMode(char letter, const char name[]) : Mode(letter, name) {}

    int i = 270;
    void init() {
        deviceBegin();
    }
    void before() {
    }

    void loop() {
        int angle1, angle2, time;
        angle1 = AbsEncorders.readDegree(0);     // 0番目のエンコーダの角度を取得
        angle2 = AbsEncorders.readDegree(1);     // 1番目のエンコーダの角度を取得
        float vel = AbsEncorders.getVelocity(1); // 1番目のエンコーダの角速度を取得[rad/s]
        time = Time.read_us();
        Time.reset(); //
        Serial.printf("%d, %d, vel:%.2f, time:%dus\n", angle1, angle2, vel, time);
    }

    void after() {
    }

  private:
};

extern MainMode mainMode;

#endif