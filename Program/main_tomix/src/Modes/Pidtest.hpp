#ifndef __Pidtest__
#define __Pidtest__

#include <./Modes/Mode.hpp>

class Pidtest : public Mode, Robot {
    using Robot::Robot;

  public:
    Pidtest(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("before:%d\n", getModeLetter());
    }

    void loop() {
        analogWrite(CorePins::MotorA, 0);
        delay(100);
        Time.reset();
        interval.reset();

        pre_w = 0;

        // ここでPIDに則ったモータの動きをする
        while (Time.read_ms() / 1000 < 10) { // 実行時間：10s
            if (interval.read_ms() >= 10) {  // 10msごとに実行
                interval.reset();
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
    float signal;
    float pre_w;
    float Kp = 1, Ki = 0.5, Kd = 1;
};

extern Pidtest pidtest;

#endif