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
        pid.setLimit(0, 12); // 出力の最大値を設定
        pid.setGain(Kp, Ki, Kd);
    }
    void before() {
        float zeroPos[2] = {0};
        for (size_t i = 0; i < 30; i++) {
            zeroPos[0] = AbsEncorders.setZero(0);
            zeroPos[1] = AbsEncorders.setZero(1);
        }
    }

    void loop() {
        // float angle0 = AbsEncorders.readDegree(0);
        // float vel0 = AbsEncorders.getVelocity(0);
        // float angleC = AbsEncorders.getContinuousDegree(0);
        // Serial.printf("%.02f, %.02f, %.02f\n", angle0, vel0, angleC);

        Serial.printf("%f, %d\n", AbsEncorders.getContinuousDegree(0), AbsEncorders.getRotationCount(0));
        delay(1);
        // float vel0 = AbsEncorders.getVelocity(0);
        // float vel1 = AbsEncorders.getVelocity(1);
        // float continuousAngle0 = AbsEncorders.getContinuousDegree(0);
        // float continuousAngle1 = AbsEncorders.getContinuousDegree(1);
        // continuousAngle0 += abs(int(continuousAngle0 / 360));
        // continuousAngle1 += abs(int(continuousAngle1 / 360));
        // Serial.printf("%.02f, %.02f, %d, %d\n", angle0, angle1, AbsEncorders.count[0], AbsEncorders.count[1]);

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
    const float Ki = 0;
    const float Kd = 0.001;
    const float dt = 0.005;
    int i = 0;
};

#endif