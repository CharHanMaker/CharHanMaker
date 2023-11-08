#include "Arduino.h"

#include "RPi_Pico_TimerInterrupt.h"

// タイマー割り込みのオブジェクトを生成する
RPI_PICO_Timer ITimer0(0);
RPI_PICO_Timer ITimer1(1);
RPI_PICO_Timer ITimer3(3);

// ピン番号を定義する
#define LED_DEBUG 16
#define LED_BUILTIN 25
#define LED_ALIVE 17

// タイマー割り込みハンドラを定義する(今回の中身は Lチカ関数)
bool TimerHandler0(struct repeating_timer *t) {
    (void)t;
    digitalWrite(LED_DEBUG, digitalRead(LED_DEBUG) ^ 1);
    return true;
}

bool TimerHandler1(struct repeating_timer *t) {
    (void)t;
    digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ^ 1);
    return true;
}

bool TimerHandler2(struct repeating_timer *t) {
    (void)t;
    digitalWrite(LED_ALIVE, digitalRead(LED_ALIVE) ^ 1);
    return true;
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_DEBUG, OUTPUT);
    pinMode(LED_ALIVE, OUTPUT);

    Serial.begin(115200);

    // 設定とかをプリントする
    Serial.println(BOARD_NAME);
    Serial.println(RPI_PICO_TIMER_INTERRUPT_VERSION);
    Serial.print(F("CPU Frequency = "));
    Serial.print(F_CPU / 1000000);
    Serial.println(F(" MHz"));

    // タイマー割り込みハンドラの設定 0.5s
    if (ITimer0.attachInterruptInterval(0.5 * 1000 * 1000, TimerHandler0)) {
        Serial.print(F("attach!!"));
    } else
        Serial.println(F("Can't set ITimer0. Select another freq. or timer")); // エラー処理

    //  タイマー割り込みハンドラの設定 1s
    if (ITimer1.attachInterruptInterval(1 * 1000 * 1000, TimerHandler1)) {
        Serial.print(F("attach!!"));
    } else
        Serial.println(F("Can't set ITimer1. Select another freq. or timer")); // エラー処理

    // タイマー割り込みハンドラの設定 0.25s
    if (ITimer3.attachInterruptInterval(0.125 * 1000 * 1000, TimerHandler2)) {
        Serial.print(F("attach!!"));
    } else
        Serial.println(F("Can't set ITimer3. Select another freq. or timer")); // エラー処理
}

void loop() {
    delay(1000);
    Serial.println("loop");
}