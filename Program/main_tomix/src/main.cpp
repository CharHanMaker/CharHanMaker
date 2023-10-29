#include <Wire.h>
#include <timer.h>
#include "MultipleAS5600.hpp"
timer Time;
MultipleAS5600 AbsEncorders(Wire, 0x70, 2);
void setup() {
    Wire.setClock(400000); // I2C の通信速度 : 400k Hz
    Wire.begin();
    Serial.begin(9600);
    AbsEncorders.begin();
}

void loop() {
    Time.reset();
    int angle1, angle2, time;
    angle1 = AbsEncorders.readDegree(0);
    angle2 = AbsEncorders.readDegree(1);
    time = Time.read_us();
    Serial.printf("%d, %d, %dus\n", angle1, angle2, time);
}
