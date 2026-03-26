#include "qtr.h"

QTRSensors::QTRSensors()
{
  // Initialize sensor values to zero
  for (int i = 0; i < QTRCount; i++)
  {
    sensorValues[i] = 0;
  }
}

void QTRSensors::setup()
{
  // Pin modes are set in setupPins()
}

void QTRSensors::read()
{
  // Read values from QTR sensors
  for (int i = 0; i < QTRCount; i++)
  {
    sensorValues[i] = digitalRead(QTRPins[i]);
  }
}

int QTRSensors::getValue(int index)
{
  if (index >= 0 && index < QTRCount)
  {
    return sensorValues[index];
  }
  
  return -1; // Invalid index
}

int* QTRSensors::getAllValues()
{
  return sensorValues;
}

void QTRSensors::printAllValues()
{
  for (int i = 0; i < QTRCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }
  Serial.println();
}