#include "Robot.hpp"
#include "TickTwo.h"
PioEncoder encA1(CorePins::EncPortA1);
Motor motor1(CorePins::Motor1, &encA1);

TickTwo tick1(
    [](){
    Serial.println("Hello!");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));}
    , 500);

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    delay(100);


    tick1.start();
}

void loop() {
    delay(1000);
}

// タイマー系タスクはloop1に書いて，loopでは時間がかかるタスクを処理する．
// デュアルコアを用いる．
void loop1() {
    tick1.update();
}