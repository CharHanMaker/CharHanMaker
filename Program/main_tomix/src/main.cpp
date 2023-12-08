#include <Robot.hpp>
#include <./Modes/MainMode.hpp>
#include <./Modes/ModeTemp.hpp>
#include <./Modes/ControlMotorMode.hpp>

#define MODE_QTY 3

MainMode mainMode('M', "MainMode");
TempMode tempMode('T', "TempMode");
ControlMotorMode controlMode('C', "ControlMotorMode");

Mode *modes[MODE_QTY] = {&mainMode, &tempMode, &controlMode};
Mode *currentMode = &tempMode;

void setup() {
    mainMode.init();
    tempMode.init();
    controlMode.init();
}

void loop() {
    if (Serial.available() > 0) {
        char c = Serial.read();

        for (unsigned i = 0; i < MODE_QTY; ++i) {
            if (c == modes[i]->getModeLetter() && currentMode != modes[i]) {
                currentMode->after();
                currentMode = modes[i];
                currentMode->before();
                login1();
            }
        }
    }
    currentMode->loop();
}
