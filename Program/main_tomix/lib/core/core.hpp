#ifndef __CORE__
#define __CORE__

#include <Arduino.h>


// Core Pins
namespace CorePins {
// UART
const uint8_t TX0 = 0;
const uint8_t RX0 = 1;

// I2C
const uint8_t SDA = 4;
const uint8_t SCL = 5;
// Encorder
const uint8_t EncPortA1 = 6;
const uint8_t EncPortB1 = 7;
const uint8_t EncPortA2 = 8;
const uint8_t EncPortB2 = 9;

// Switch
const uint8_t ENG_SW = 10;
const uint8_t SW = 11;

// LED
const uint8_t Debug_LED = 16;
const uint8_t Alive_LED = 17;

// Servo
const uint8_t Servo1 = 18;
const uint8_t Servo2 = 19;
const uint8_t Servo3 = 22;

// PWM(Motor)
const uint8_t Motor1 = 20;
const uint8_t Motor2 = 21;

// ADC
const uint8_t ADC_0 = 26;
const uint8_t ADC_1 = 27;
const uint8_t ADC_2 = 28;

} // namespace CorePins

#endif