#ifndef __MODE__
#define __MODE__

#include <Robot.hpp>

class Mode {
  public:
    Mode(char letter, const char name[]) : modeLetter(letter) {
        strcpy(modeName, name);
    }

    char *getModeName() {
        return modeName;
    }

    char getModeLetter() {
        return modeLetter;
    }

    virtual void init() {
        Serial.printf("Resistored : '%c' as %s\n", getModeLetter(), getModeName());
    };
    virtual void before() {
        Serial.printf("before %s\n", getModeName());
    };
    virtual void loop() {
        Serial.printf("loop %s\n", getModeName());
    };
    virtual void after() {
        Serial.printf("after %s\n", getModeName());
    };

  protected:
    char modeName[24];
    char modeLetter;
};

#endif