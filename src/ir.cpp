#include "ir.h"

IRSensors::IRSensors()
{
  // Initialize sensor values to zero
  for (int i = 0; i < IRCount; i++)
  {
    sensorValues[i] = 0;
  }
}

void IRSensors::setup()
{
  // Pin modes are set in setupPins()
}

void IRSensors::read()
{
  // Read values from IR sensors
  for (int i = 0; i < IRCount; i++)
  {
    sensorValues[i] = digitalRead(IRPins[i]);
  }
}

int IRSensors::getValue(int index)
{
  if (index >= 0 && index < IRCount)
  {
    return sensorValues[index];
  }

  return -1; // Invalid index
}

int *IRSensors::getAllValues()
{
  return sensorValues;
}

void IRSensors::printAllValues()
{
  for (int i = 0; i < IRCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }
  Serial.println();
}