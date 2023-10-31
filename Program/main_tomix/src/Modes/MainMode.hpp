#ifndef __MAIN__
#define __MAIN__

#include <./Modes/Mode.hpp>

class MainMode : public Mode, Robot {
    using Robot::Robot;

  public:
    MainMode(char letter, const char name[]) : Mode(letter, name) {}

    int i = 0;

    void init() {
		
        deviceBegin();

		
    }

    void before() {
    }

    void loop() {
        i++;
        int power = 65535 / 2 * MyMath::sinDeg(i) + 65535 / 2;
        analogWrite(CorePins::MotorA, power);
        analogWrite(CorePins::MotorB, power);
        delay(10);
        Serial.printf("i:%d\n", power);
    }

    void after() {
    }

  private:
};

extern MainMode mainMode;

#endif