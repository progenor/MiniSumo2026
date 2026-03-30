#include "ir.h"

IRSensors::IRSensors()
{
  // Initialize sensor values and debounce counters to zero
  for (int i = 0; i < IRCount; i++)
  {
    sensorValues[i] = 0;
    rawValues[i] = 0;
    debounceCounter[i] = 0;
  }
}

void IRSensors::setup()
{
  // Pin modes are set in setupPins()
}

void IRSensors::read()
{
  // Read raw values from IR sensors with debounce filtering
  // Debounce Filter: Requires DEBOUNCE_THRESHOLD consecutive stable readings
  // before accepting a state change. This filters out transient spikes from
  // ground bounce during fast turns and other noise.

  for (int i = 0; i < IRCount; i++)
  {
    int rawRead = digitalRead(IRPins[i]);
    rawValues[i] = rawRead;

    // Check if raw reading matches current filtered value
    if (rawRead == sensorValues[i])
    {
      // Stable state - reset debounce counter
      debounceCounter[i] = 0;
    }
    else
    {
      // State change detected - increment debounce counter
      debounceCounter[i]++;

      // Accept state change only after threshold is reached
      if (debounceCounter[i] >= DEBOUNCE_THRESHOLD)
      {
        sensorValues[i] = rawRead; // Update filtered value
        debounceCounter[i] = 0;    // Reset counter
      }
    }
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