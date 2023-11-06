#include <Robot.hpp>
#include <./Modes/MainMode.hpp>
#include <./Modes/ModeTemp.hpp>
#include <./Modes/SystemIdentification.hpp>

#define MODE_QTY 3

MainMode mainMode('M', "MainMode");
TempMode tempMode('T', "TempMode");
SystemIdentificationMode systemidentificationMode('S', "SysIdMode");

Mode *modes[MODE_QTY] = {&mainMode, &tempMode, &systemidentificationMode};
Mode *currentMode = &tempMode;

void setup() {
    mainMode.init();
    mainMode.before();
}

void loop() {
    if (Serial.available() > 0) {
        char c = Serial.read();
        for (unsigned i = 0; i < MODE_QTY; ++i) {
            if (c == modes[i]->getModeLetter() && currentMode != modes[i]) {
                currentMode->after();
                currentMode->before();
                currentMode = modes[i];
            }
        }
    }
    currentMode->loop();
}
