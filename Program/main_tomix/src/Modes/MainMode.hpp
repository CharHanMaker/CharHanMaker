#ifndef __MAIN__
#define __MAIN__

#include <./Modes/Mode.hpp>

class MainMode : public Mode, Robot {
    using Robot::Robot;

  public:
    MainMode(char letter, const char name[]) : Mode(letter, name) {}
    void init() {
        deviceBegin();
        for (size_t i = 0; i < 10; i++) {
            AbsEncorders.setZero(0);
            AbsEncorders.setZero(1);
        }
    }

    void before() {
        i = 0;
        login1();
    }

    void loop() {
        // // モータを回すプログラムです．
        int run = 65535 * MyMath::sinDeg(i);
        motorA.runOpenloop(run, true); // これで無理やりモータを回せます．
        motorB.runOpenloop(run, true); // これで無理やりモータを回せます．
        delay(15);
        Serial.println(run);
        i++;
    }

    void after() {
        motorA.stop();
        motorB.stop();
    }

  private:
    int i = 0;
};

#endif