#ifndef __TempMode__
#define __TempMode__

#include <./Modes/Mode.hpp>

class TempMode : public Mode, Robot {
    using Robot::Robot;

  public:
    TempMode(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("loop %s\n", getModeName());
    }

    void loop() {
        Serial.printf("loop %s waiting for send 'M\n", getModeName());
        delay(1000);
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
        motorA.stop();
        motorB.stop();
    }

  private:
};

extern TempMode tempMode;

#endif