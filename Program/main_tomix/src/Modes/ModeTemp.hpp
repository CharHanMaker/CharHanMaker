#ifndef __TempMode__
#define __TempMode__

#include <./Modes/Mode.hpp>

class TempMode : public Mode, Robot {
    using Robot::Robot;

  public:
    TempMode(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("before:%d", getModeLetter());
    }

    void loop() {
        Serial.printf("loop:%d", getModeLetter());
    }

    void after() {
        Serial.printf("after:%d", getModeLetter());
    }

  private:
};

extern TempMode tempMode;

#endif