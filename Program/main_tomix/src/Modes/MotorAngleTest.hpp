#ifndef __MotorAngleTest__
#define __MotorAngleTest__

#include <./Modes/Mode.hpp>

class MotorAngleTest : public Mode, Robot {
    using Robot::Robot;

  public:
    MotorAngleTest(char letter, const char name[]) : Mode(letter, name) {}

    void before() {
        Serial.printf("loop %s\n", getModeName());
        /*　--------------------EEPROMからエンコーダの初期角度の読み込み-------------------- */
        // これをbeforeに書かないと初期角度をセットしないままで位置制御始まるので危ないよお
        Serial.println("===========================================");
        Serial.println("Read from EEPROM");
        uint16_t rawValue0, rawValue1;
        readEncoderZeroPos(rawValue0, rawValue1); // EEPROMから初期角度値の読み出し
        Serial.printf("ZeroDeg0:%.2f, ZeroDeg1:%.2f\n", float(rawValue0 * BIT_12_TO_DEGREE), float(rawValue1 * BIT_12_TO_DEGREE));
        AbsEncorders.setZero(0, rawValue0); // EEPROMに保存されていてたエンコーダの初期角度を使用する
        AbsEncorders.setZero(1, rawValue1); // EEPROMに保存されていてたエンコーダの初期角度を使用する
        Serial.println("===========================================");
        // ここまで
        /*　--------------------EEPROMからエンコーダの初期角度の読み込み-------------------- */
        login1();

        motorA.setSaturation(65535, -65535);
        motorB.setSaturation(65535, -65535);

        velPID[0].setLimit(-12, 12);
        velPID[0].reset();
        velPID[1].setLimit(-12, 12);
        velPID[1].reset();

        angPID[0].setLimit(-6, 6);
        angPID[0].reset();
        angPID[1].setLimit(-6, 6);
        angPID[1].reset();

        Serial.setTimeout(5);
        isLoop = true;

        turnMotorZeroPos();
    }

    void loop() {
        while (isLoop) {
            if (Serial.available() > 0) {
                getSerialData();
            } else {
                getSensors();

                motor[1]->runOpenloop(voltToDuty(1), true);
                angleCtrl();
                // velCtrl();
                // for (size_t i = 0; i < 2; i++){
                //     printVal(i,angle[i].current);
                // }
                // Serial.println();
            }
        }

        Serial.println("Send 'T' to reset");
        delay(1000);
    }

    void velCtrl(){
        for (size_t i = 0; i < 2; i++){
            vel[i].error = -(vel[i].target - vel[i].current);        // これはmotorA.setCW()でなくせるはず
            velPID[i].appendError(vel[i].error);
            velPID[i].compute();
            vel[i].output = velPID[i].getPID();
            motor[i]->runOpenloop(voltToDuty(vel[i].output), true);
        }
        
        for (size_t i = 0; i < 2; i++){
            Serial.printf("targetVel:%.2f, currentVel:%.2f, error:%.2f, output:%.2f\n", vel[i].target, vel[i].current, vel[i].error, vel[i].output);
        }
    }

    void getSensors(){
        for (size_t i = 0; i < 2; i++){
            // prev
            angle[i].prev = angle[i].current;
            angleContinuous[i].prev = angleContinuous[i].current;
            vel[i].prev = vel[i].current;

            // get Sensors
            angle[i].current = AbsEncorders.readRadian(i);
            angleContinuous[i].current = AbsEncorders.getContinuousRadian(i);
            vel[i].current = AbsEncorders.getVelocity(i); //[rad/s]            
        }
    }

    void angleCtrl(){
        // angleContinuous[0].target = 0; //目標値
        angleContinuous[0].target = angleContinuous[1].current; //目標値
        angleContinuous[0].error = (angleContinuous[0].target - angleContinuous[0].current); //偏差
        angPID[0].appendError(angleContinuous[0].error);
        angPID[0].compute();
        angleContinuous[0].output = angPID[0].getPID();
        motor[0]->runOpenloop(voltToDuty(angleContinuous[0].output), true);
        
        // for (size_t i = 0; i < 1; i++){
        //     Serial.printf("targetAng:%.2f, currentAng:%.2f, error:%.2f, output:%.2f\n", angleContinuous[i].target, angleContinuous[i].current, angleContinuous[i].error, angleContinuous[i].output);
        // }
        Serial.printf("currentAng1:%.2f, vel1:%.2f, currentAng2:%.2f, vel2:%.2f\n", angleContinuous[0].current, vel[0].current, angleContinuous[1].current, vel[1].current);
    }

    void getSerialData(){
        String str = Serial.readStringUntil('\n'); // 改行までの文字列を受信
        if (str == "stop") {                       // stopと入力されたらモータを止める
            isLoop = false;                        // whileループを抜ける
            motorA.stop();
            motorB.stop();
            logout1();
        } else if (str.startsWith("set")) {
            str.remove(0, 3); // "set"の3文字部分を削除
            vel[0].target = str.toFloat();
            vel[1].target = vel[0].target;
            // velPID.resetIntegral();
        }
    }

    void turnMotorZeroPos(){
        AbsEncorders.readDegree(0);
        AbsEncorders.readDegree(1);
        while (abs(AbsEncorders.readDegree(0)) > 0.1) {
            AbsEncorders.readDegree(0);
            AbsEncorders.readDegree(1); 
            motorA.runOpenloop(voltToDuty(1), true);
        }
        motorA.stop();
        AbsEncorders.resetRotationCount(0);

        AbsEncorders.readDegree(0);
        AbsEncorders.readDegree(1);
        while (abs(AbsEncorders.readDegree(1)) > 0.1) {
            AbsEncorders.readDegree(0);
            AbsEncorders.readDegree(1); 
            motorB.runOpenloop(voltToDuty(1), true);
        }
        motorB.stop();
        AbsEncorders.resetRotationCount(1);
    }

    void printVal(int i,float data){
        Serial.printf("angle:[%d]:%.2f ",i,data);
    }

    void after() {
        Serial.printf("after %s\n", getModeName());
        motorA.stop();
        motorB.stop();
    }

  private:
    // typedefs 
    typedef struct{
        float Kp;
        float Ki;
        float Kd;
        float dt;
    } Gain;

    typedef struct{
        float target;
        float current;
        float prev;
        float error;
        float output;
    }ctrlValues;

    // values for control
    ctrlValues angle[2];
    ctrlValues angleContinuous[2];
    ctrlValues vel[2];

    // Motor def
    MotorPid *motor[2] = {&motorA,&motorB};
    
    // Gains
    Gain velGain = {
        .Kp = 0.0,
        .Ki = 1.0,
        .Kd = 0.0,
        .dt = 0.003
    };

    Gain angGain = {
        .Kp = 7.0,
        .Ki = 0.005,
        .Kd = 0.005,
        .dt = 0.003
    };

    // PID Objects
    PID velPID[2] = {
                        PID(velGain.Kp, velGain.Ki, velGain.Kd, velGain.dt),
                        PID(velGain.Kp, velGain.Ki, velGain.Kd, velGain.dt)
                    };
    PID angPID[2] = {
                        PID(angGain.Kp, angGain.Ki, angGain.Kd, angGain.dt),
                        PID(angGain.Kp, angGain.Ki, angGain.Kd, angGain.dt)
                    };

    bool isLoop = true;

    // functions
    int32_t voltToDuty(float volt) {
        return int32_t(volt * 65535 / 12);
    }
};

extern MotorAngleTest motorAngleTest;

#endif