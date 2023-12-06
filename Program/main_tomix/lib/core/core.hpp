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

// Switch
const uint8_t ENG_SW = 2;
const uint8_t SW = 3;

// LED
const uint8_t Debug_LED = 16;
const uint8_t Alive_LED = 17;

// Servo
const uint8_t Servo1 = 18;
const uint8_t Servo2 = 19;
const uint8_t Servo3 = 20;

// Buzzer
const uint8_t Buzzer = 21;

// PWM(Motor)
const uint8_t MotorPHA = 7;
const uint8_t MotorPHB = 9;
const uint8_t MotorENA = 6;
const uint8_t MotorENB = 8;

// ADC
const uint8_t ADC_0 = 26;
const uint8_t ADC_1 = 27;
const uint8_t ADC_2 = 28;

} // namespace CorePins

#endif