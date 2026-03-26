#ifndef PINS_H
#define PINS_H

#include <Arduino.h>
#include "defines.h"

// Motor Control Pins
#define PWM_A1 9   // GP9 controls Driver A IN1
#define PWM_A2 8   // GP8 controls Driver A IN2
#define PWM_B1 20  // GP20 controls Driver B IN1
#define PWM_B2 21  // GP21 controls Driver B IN2
#define N_SLEEP 22 // GP22 controls both drivers' nSLEEP pins

// Buzzer Pin
#define BUZZER 3

// IR Sensor Pins
#define SENSOR_LEFT 14
#define SENSOR_CENTER 13
#define SENSOR_RIGHT 12

// QTR Sensor Pins
#define QTR_LEFT 2
#define QTR_RIGHT 3

const uint8_t IRPins[] = {SENSOR_LEFT, SENSOR_CENTER, SENSOR_RIGHT};
const uint8_t QTRPins[] = {QTR_LEFT, QTR_RIGHT};

void setupPins();

#endif // PINS_H