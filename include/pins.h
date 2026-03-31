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

// Current Sensing Pin (IPROPI)
#define IPROPI_A_PIN 26 // GP26 ADC input for motor A current sensing
#define IPROPI_B_PIN 27 // GP27 ADC input for motor B current sensing

// IR Sensor Pins
#define SENSOR_LEFT 12
#define SENSOR_CENTER 11
#define SENSOR_RIGHT 10

// QTR Sensor Pins
#define QTR_LEFT 10
#define QTR_RIGHT 11

// Button Pin
#define BUTTON_PIN 15

const uint8_t IRPins[] = {SENSOR_LEFT, SENSOR_CENTER, SENSOR_RIGHT};
const uint8_t QTRPins[] = {QTR_LEFT, QTR_RIGHT};

void setupPins();

#endif // PINS_H