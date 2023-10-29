#ifndef __MAIN__
#define __MAIN__

#include <./Modes/Mode.hpp>

class MainMode : public Mode, Devices {
    using Devices::Devices;

  public:
    MainMode(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        deviceBegin();
    }
    void loop() {
        int angle1, angle2, time;
        angle1 = AbsEncorders.readDegree(0);     // 0番目のエンコーダの角度を取得
        angle2 = AbsEncorders.readDegree(1);     // 1番目のエンコーダの角度を取得
        float vel = AbsEncorders.getVelocity(1); // 1番目のエンコーダの角速度を取得[rad/s]
        time = Time.read_us();
        Time.reset(); //
        a++;
        Serial.printf("%d, %d, vel:%.2f, time:%dus %d\n", angle1, angle2, vel, time, a);
    }
    void after() {
    }

  private:
};

extern MainMode mainMode;

#endif