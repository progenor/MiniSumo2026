#ifndef IR_H
#define IR_H

#include "defines.h"
#include "Arduino.h"
#include "pins.h"

#define IRCount 3 // Number of IR sensors
//extern const uint8_t IRPins[];

class IRSensors
{
public:
  IRSensors();

  void setup();

  void read();

  int getValue(int index);

  int *getAllValues();

  void printAllValues();

private:
  int sensorValues[IRCount];
};

#endif
