#include <Robot.hpp>
#include <./Modes/MainMode.hpp>

MainMode mainMode('M', "MainMode");

void setup() {
    Wire.setClock(400000); // I2C の通信速度 : 400k Hz
    Wire.begin();
    Serial.begin(9600);

    mainMode.init();
    mainMode.before();
}

void loop() {
    mainMode.loop();
}
