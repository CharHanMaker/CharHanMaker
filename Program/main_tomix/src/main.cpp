#include "Robot.hpp"

PioEncoder encA1(CorePins::EncPortA1);
Motor motor1(CorePins::Motor1, &encA1);

void setup() {
    motor1.begin();
    motor1.setGain(1, 1, 0);
    motor1.setLimit(120, -120);
}
void loop() {
    motor1.drive(100);
}