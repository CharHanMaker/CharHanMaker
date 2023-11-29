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
        float angle0 = AbsEncorders.readDegree(0);
        float angle1 = AbsEncorders.readDegree(1);
        float vel0 = AbsEncorders.getVelocity(0);
        float vel1 = AbsEncorders.getVelocity(0);
        Serial.printf("%f, %f, %f, %f\n", angle0, angle1, vel0, vel1);

        // // モータを回すプログラムです．
        // int run = 65535 / 2 * MyMath::sinDeg(i);
        // motorA.runOpenloop(run, true); // これで無理やりモータを回せます．
        // // mototA.drive(velocity);// :TODO:@ryoskRFR ←←ここにPID制御の速度制御を入れたいよね
        // delay(50);
        // Serial.println(run);
        // i++;
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