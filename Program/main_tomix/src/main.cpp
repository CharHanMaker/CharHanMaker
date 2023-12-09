#include <Robot.hpp>
#include <./Modes/MainMode.hpp>
#include <./Modes/ModeTemp.hpp>
#include <./Modes/SensorTest.hpp>
#include <./Modes/MotorVelTest.hpp>

#define MODE_QTY 4

MainMode mainMode('M', "MainMode");
TempMode tempMode('T', "TempMode");
SensorTestMode sensorTestMode('S', "SensorTest");
MotorVelTest motorVelTest('V', "MotorVelTest");

Mode *modes[MODE_QTY] = {&mainMode, &tempMode, &sensorTestMode, &motorVelTest};
Mode *currentMode = &tempMode;

void setup() {
    mainMode.init();
    tempMode.init();
}

void loop() {
    if (Serial.available() > 0) {
        char c = Serial.read();
        for (unsigned i = 0; i < MODE_QTY; ++i) {
            if (c == modes[i]->getModeLetter() && currentMode != modes[i]) {
                currentMode->after();
                currentMode = modes[i];
                currentMode->before();
            }
        }
    }
    currentMode->loop();
}
