#include <Robot.hpp>
#include <./Modes/MainMode.hpp>
#include <./Modes/ModeTemp.hpp>

#define MODE_QTY 2

MainMode mainMode('M', "MainMode");
TempMode tempMode('T', "TempMode");

Mode *modes[MODE_QTY] = {&mainMode, &tempMode};
Mode *currentMode = &mainMode;

void setup() {
    Wire.setClock(400000); // I2C の通信速度 : 400k Hz
    Wire.begin();
    Serial.begin(115200);

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
