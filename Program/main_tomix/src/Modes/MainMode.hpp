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
        // i++;
        // int power = 65535 / 2 * MyMath::sinDeg(i) + 65535 / 2;
        // analogWrite(CorePins::MotorA, power);
        // analogWrite(CorePins::MotorB, power);
        // delay(10);
        // Serial.printf("i:%d\n", power);
		analogWrite(CorePins::MotorA, 0);
		delay(100);
		Time.reset();
		for(unsigned i = 0; i < 10000; ++i) {
			analogWrite(CorePins::MotorA, 65535);
			Serial.printf("%d,%f,%d\n",Time.read_us(),AbsEncorders.readDegree(0),AbsEncorders.getVelocity(0));
		}
		analogWrite(CorePins::MotorA, 0);
		Serial.printf("end...Send T to continue\n");
		delay(10000);
    }

    void after() {
    }

  private:
};

extern MainMode mainMode;

#endif