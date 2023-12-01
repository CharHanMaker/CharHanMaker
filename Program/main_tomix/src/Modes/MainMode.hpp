#ifndef __MAIN__
#define __MAIN__

#include <./Modes/Mode.hpp>
#include <PID.hpp>

class MainMode : public Mode, Robot {
    using Robot::Robot;

  public:
    MainMode(char letter, const char name[]) : Mode(letter, name), pid(Kp, Ki, Kd, dt) {}
    int i = 270;
    void init() {
        deviceBegin();
        pid.setLimit(0, 12); // 出力の最大値を設定
        pid.setGain(Kp, Ki, Kd);
    }
    void before() {
    }

    void loop() {
        //:TODO: @ryoskRFR チェックプリーズ
        // コメントアウトしながらチェックしたい(特にgetContinuousDegreeとgetContinuousRadian)
        // radianとdegreeの違いも．
        //　角度
        float degree0 = AbsEncorders.readDegree(0); // 0~360
        float degree1 = AbsEncorders.readDegree(1);
        float radian0 = AbsEncorders.readDegree(0); // 0~2π
        float radian1 = AbsEncorders.readDegree(1);
        Serial.printf("DEGREE: %f, %f, %f, %f \n", degree0, degree1, radian0, radian1);

        // 角速度
        float radPerSec0 = AbsEncorders.getVelocity(0);
        float radPerSec1 = AbsEncorders.getVelocity(0);
        Serial.printf("RAD/S: %f, %f \n", radPerSec0, radPerSec1);

        // 連続した角度
        float continuouseDegree0 = AbsEncorders.getContinuousDegree(0);
        float continuouseDegree1 = AbsEncorders.getContinuousDegree(1);
        float continuouseRadian0 = AbsEncorders.getContinuousRadian(0);
        float continuouseRadian1 = AbsEncorders.getContinuousRadian(1);
        Serial.printf("CONTINUE:%f, %f, %f, %f \n", continuouseDegree0, continuouseDegree1, continuouseRadian0, continuouseRadian1);

        
        delay(10);
    }

    void after() {
        I = 0;
        analogWrite(CorePins::MotorA, 0);
    }

  private:
    PID pid;
    const float Kp = 1;
    const float Ki = 0;
    const float Kd = 0.001;
    const float dt = 0.005;
    float target = 10;
    timer Time;
    float I;
};

extern MainMode mainMode;

#endif