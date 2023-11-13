#include <Robot.hpp>
#include <./Modes/MainMode.hpp>
#include <./Modes/ModeTemp.hpp>
#include <./Modes/SystemIdentification.hpp>
#include <./Modes/SystemIdentification2.hpp>

#define MODE_QTY 4

MainMode mainMode('M', "MainMode");
TempMode tempMode('T', "TempMode");
SystemIdentificationMode systemidentificationMode('S', "SI Mode1");
SystemIdentificationMode2 systemidentificationMode2('I', "SI Mode2");

Mode *modes[MODE_QTY] = {&mainMode, &tempMode, &systemidentificationMode, &systemidentificationMode2};
Mode *currentMode = &tempMode;

void setup() {
    mainMode.init();
    tempMode.init();
    systemidentificationMode.init();
}

void loop() {
    if (Serial.available() > 0) {
        // char c = Serial.read();
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
