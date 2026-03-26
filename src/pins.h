#ifndef PINS_H
#define PINS_H

#include <Arduino.h>
#include "defines.h"

// pins
#define AIN1 15
#define AIN2 14
#define PWMA 13

#define BIN1 12
#define BIN2 11
#define PWMB 10

#define BUZZER 19

#define SENSOR_CENTER 18
#define SENSOR_LEFT 17
#define SENSOR_RIGHT 16

#define QTR_LEFT 2
#define QTR_RIGHT 3

const uint8_t IRPins[] = {SENSOR_LEFT, SENSOR_CENTER, SENSOR_RIGHT};
const uint8_t QTRPins[] = {QTR_LEFT, QTR_RIGHT};

void setupPins();

#endif // PINS_H