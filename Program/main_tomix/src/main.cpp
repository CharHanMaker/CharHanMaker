#include "Robot.hpp"
#include "TickTwo.h"
PioEncoder encA1(CorePins::EncPortA1);
Motor motor1(CorePins::Motor1, &encA1);

void printMessage() {
    Serial.println("Hello!");
}

TickTwo tick1(printMessage, 0, 1);

void setup() {
    // motor1.begin();
    // motor1.setGain(1, 1, 0);
    // motor1.setLimit(120, -120);
    tick1.start();
}
void loop() {
    tick1.update();
}