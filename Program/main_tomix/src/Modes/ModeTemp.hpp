#ifndef __TempMode__
#define __TempMode__

#include <./Modes/Mode.hpp>

class TempMode : public Mode, Robot {
    using Robot::Robot;

  public:
    TempMode(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("before:%d\n", getModeLetter());
    }

    void loop() {
        Serial.printf("loop:%d\n waiting for send 'M' ", getModeLetter());
        delay(1000);
    }

    void after() {
        Serial.printf("after:%d\n", getModeLetter());
    }

  private:
};

extern TempMode tempMode;

#endif