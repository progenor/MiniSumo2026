#ifndef QTR_H
#define QTR_H

#include <Arduino.h>
#include "pins.h"
#include "defines.h"

#define QTRCount 2
//extern const uint8_t QTRPins[];

class QTRSensors
{
public:
  QTRSensors();

  void setup();

  void read();

  int getValue(int index);

  int *getAllValues();

  void printAllValues();

private:
  int sensorValues[QTRCount];
};

#endif
