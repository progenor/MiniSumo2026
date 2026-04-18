#ifndef IR_H
#define IR_H

#include "defines.h"
#include "Arduino.h"
#include "pins.h"

#define IRCount 3            // Number of IR sensors
#define DEBOUNCE_THRESHOLD 1 // Number of consecutive stable readings required to accept state change
// extern const uint8_t IRPins[];

class IRSensors
{
public:
  IRSensors();

  void setup();

  void read(); // Reads raw sensors and applies debounce filtering

  int getValue(int index);

  int *getAllValues();

  void printAllValues();

private:
  int sensorValues[IRCount];    // Filtered sensor values
  int rawValues[IRCount];       // Raw unfiltered sensor values
  int debounceCounter[IRCount]; // Debounce counter for each sensor
};

#endif
