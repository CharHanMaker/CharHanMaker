#include <Arduino.h>
#include <core.hpp>

void setup() {
    pinMode(CorePins::Motor1, OUTPUT);
    pinMode(CorePins::Motor2, OUTPUT);
    analogWriteFreq(100 * 10E3);
    analogWriteRange(4096);
}
void loop() {
    // sine wave with diff pi
    for (int i = 0; i < 4096; i++) {
        analogWrite(CorePins::Motor1, 2048 + 2048 * sin(2 * PI * i / 4096));
        analogWrite(CorePins::Motor2, 2048 + 2048 * sin(2 * PI * i / 4096 + PI));
        // delay();
        Serial.printf("%d\n", i);
    }
}

// // encoder
// PioEncoder encA1(EncPortA1);
// PioEncoder encB1(EncPortB1);

// void setup() {
//     // led
//     pinMode(Debug_LED, OUTPUT);
//     pinMode(Alive_LED, OUTPUT);
//     // Servo
//     pinMode(Servo1, OUTPUT);
//     pinMode(Servo2, OUTPUT);
//     pinMode(Servo3, OUTPUT);
//     // Motor
//     pinMode(Motor1, OUTPUT);
//     pinMode(Motor2, OUTPUT);
//     // Encoder
//     encA1.begin();
//     encB1.begin();

//     // switch
//     pinMode(ENG_SW, INPUT);
//     pinMode(SW, INPUT);
//     // ADC
//     pinMode(ADC_zero, INPUT);
//     pinMode(ADC1, INPUT);
//     pinMode(ADC2, INPUT);

//     Serial.begin(115200);
// }

// void loop() {

//     Serial.print(encA1.getCount());
//     Serial.print("\t");

//     Serial.print(encB1.getCount());
//     Serial.print("\n");

//     delay(10);
// }

// #include "pio_encoder.h"

// PioEncoder encoder(6); // encoder is connected to GPIO2 and GPIO3

// void setup() {
//     encoder.begin();
//     Serial.begin(115200);
// }

// void loop() {
//     Serial.println(encoder.getCount());
//     delay(10);
// }