#ifndef __SYSID__
#define __SYSID__

#include <./Modes/Mode.hpp>

class SystemIdentificationMode : public Mode, Robot {
    using Robot::Robot;

  public:
    SystemIdentificationMode(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("before:%d\n", getModeLetter());
    }

    void loop() {
        // Serial.printf("loop:%d\n waiting for send 'M' ", getModeLetter());
        // delay(1000);

        Serial.println("Time[us], Volt[V], Angle[rad], AngleVel[rad/s]");
        // initialize
        analogWrite(CorePins::MotorA, 0);
        delay(100);
        Time.reset();

        for (unsigned i = 1; i <= 10000; i++) {
            float freq = i * (100 - 0.1) / 10000 + 0.1;
            float Volt = 65535 * sin(2 * PI * Time.read_us() * freq);
            analogWrite(CorePins::MotorA, Volt);
            Serial.printf("%d, %f, %f, %f\n", Time.read_us(), Volt, AbsEncorders.readDegree(0), AbsEncorders.getVelocity(0));
        }

        analogWrite(CorePins::MotorA, 0);
        Serial.printf("end...Send T to continue\n");
        delay(10000);
    }

    void after() {
        Serial.printf("after:%d\n", getModeLetter());
    }

  private:
};

extern SystemIdentificationMode systemidentificationMode;

#endif