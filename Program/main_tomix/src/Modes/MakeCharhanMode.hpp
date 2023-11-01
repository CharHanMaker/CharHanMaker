#ifndef __TempMode__
#define __TempMode__

#include <./Modes/Mode.hpp>

class TempMode : public Mode, Robot {
    using Robot::Robot;

  public:
    TempMode(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("before:%s\n", getModeLetter());
    }

    void loop() {
        Serial.printf("loop:%s\n", getModeLetter());
        analogWrite(CorePins::MotorA, 0);
    }

    void after() {
        Serial.printf("after:%s\n", getModeLetter());
    }

  private:
};

extern TempMode tempMode;

#endif