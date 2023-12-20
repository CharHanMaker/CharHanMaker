#include <Robot.hpp>
#include <./Modes/MainMode.hpp>
#include <./Modes/ModeTemp.hpp>
#include <./Modes/SensorTest.hpp>
#include <./Modes/MotorVelTest.hpp>
#include <./Modes/ControlMotorMode.hpp>
#include <./Modes/SystemIdentification.hpp>
#include <./Modes/SynchronizeTest.hpp>

#define MODE_QTY 7

MainMode mainMode('M', "MainMode");
TempMode tempMode('T', "TempMode");
SensorTestMode sensorTestMode('S', "SensorTest");
MotorVelTest motorVelTest('V', "MotorVelTest");
ControlMotorMode controlMode('C', "ControlMotorMode");
SystemIdentificationMode systemidentificationMode('I', "システム同定");
SynchronizeTest synchronizeTest('Y', "SynchronizeTest");

Mode *modes[MODE_QTY] = {&mainMode, &tempMode, &sensorTestMode, &motorVelTest, &controlMode, &systemidentificationMode, &synchronizeTest};
Mode *currentMode = &tempMode;

void setup() {
    for (size_t i = 0; i < MODE_QTY; i++) {
        modes[i]->init();
    }
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
