#include "Robot.hpp"

// 以下のコメントアウトを外すと、EEPROMの読み書きを行う
#define EEPROM_READ
// #define EEPROM_WRITE

#ifdef EEPROM_READ
#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
int address = 0;
byte value;

void setup() {
    // initialize serial and wait for port to open:
    Serial.begin(115200);
    EEPROM.begin(512);
}

void loop() {
    // read a byte from the current address of the EEPROM
    value = EEPROM.read(address);

    Serial.print(address);
    Serial.print("\t");
    Serial.print(value, DEC);
    Serial.println();

    // advance to the next address of the EEPROM
    address = address + 1;

    // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
    // on address 512, wrap around to address 0
    if (address == 512) {
        address = 0;
    }

    delay(10);
}

#endif

#ifdef EEPROM_WRITE
/*
   EEPROM Write

   Stores values read from analog input 0 into the EEPROM.
   These values will stay in the EEPROM when the board is
   turned off and may be retrieved later by another sketch.
*/

#include <EEPROM.h>

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addr = 0;

void setup() {
    Serial.begin(115200);
    EEPROM.begin(512);
}

void loop() {
    for (uint8_t i = 0; i < 255; i++) {
        EEPROM.write(addr, 255 - i);
        addr = addr + 1;
        if (addr == 255) {
            addr = 0;
            if (EEPROM.commit()) {
                Serial.println("EEPROM successfully committed");
            } else {
                Serial.println("ERROR! EEPROM commit failed");
            }
        }
        delay(50);
    }
}

#endif