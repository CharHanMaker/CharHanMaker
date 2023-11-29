#ifndef __MAIN__
#define __MAIN__

#include <./Modes/Mode.hpp>
#include <PID.hpp>

class MainMode : public Mode, Robot {
    using Robot::Robot;

  public:
    MainMode(char letter, const char name[]) : Mode(letter, name), pid(Kp, Ki, Kd, dt) {}
    void init() {
        deviceBegin();
        pid.setLimit(0, 65535); // 出力の最大値を設定
        pid.setGain(Kp, Ki, Kd);
    }
    void before() {
    }

    void loop() {
        int run = 65535 / 2 * MyMath::sinDeg(i) + 65535 / 2;
        motorA.runOpenloop(run);
        delay(50);
        i++;
        Serial.printf("%d\n", run);
    }

    void after() {
        analogWrite(CorePins::MotorA1, 0);
        analogWrite(CorePins::MotorA2, 0);
    }

  private:
    PID pid;
    const float Kp = 1;
    const float Ki = 1;
    const float Kd = 0;
    const float dt = 0.005;
    int i = 0;
};

#endif