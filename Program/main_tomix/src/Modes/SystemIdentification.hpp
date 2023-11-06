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

        Serial.println("Time[ms], Volt[V], Angle[rad], AngleVel[rad/s]");
        // initialize
        analogWrite(CorePins::MotorA, 0);
        delay(100);
        Time.reset();
        interval.reset();

        float freq = 0.1;

        while (freq < 100) {
            // 定期実行 おおよそ10msごとに
            if (interval.read_ms() >= 10) {
                interval.reset();
                if (freq < 1)
                    freq += 0.1;
                else if (freq < 10)
                    freq += 1;
                else if (freq < 100)
                    freq += 10;

                float Volt = 65535 * sin(2 * PI * Time.read_ms() * freq);
                Volt = Volt / 2 + 65535 / 2;
                analogWrite(CorePins::MotorA, Volt);
                Serial.printf("%d, %f, %f, %f\n", Time.read_ms(), Volt, AbsEncorders.readDegree(0), AbsEncorders.getVelocity(0));
            }
        }

        analogWrite(CorePins::MotorA, 0);
        Serial.printf("end...Send T to continue\n");
        delay(10000);
    }

    void after() {
        Serial.printf("after:%d\n", getModeLetter());
    }

  private:
    timer interval;
};

extern SystemIdentificationMode systemidentificationMode;

#endif