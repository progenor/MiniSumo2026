#include "pins.h"
#include "ir.h"
#include "qtr.h"

// Define IR configuration here to avoid multiple definitions from headers

void setupPins()
{
    // define motor control pins
    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);
    pinMode(N_SLEEP, OUTPUT);

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
