#include "pins.h"
#include "ir.h"
#include "qtr.h"

// Define IR configuration here to avoid multiple definitions from headers


void setupPins()
{
    // define motor control pins
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);

    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);

    pinMode(BUZZER, OUTPUT);

    // Initialize IR sensor pins
    for (int i = 0; i < IRCount; i++)
    {
        pinMode(IRPins[i], INPUT);
    }

    // Initialize QTR sensor pins
    for (int i = 0; i < QTRCount; i++)
    {
        pinMode(QTRPins[i], INPUT);
    }
}
