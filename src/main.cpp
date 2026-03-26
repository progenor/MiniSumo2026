
#include <Arduino.h>
#include "robot.h"

// Global robot instance
Robot robot;

void setup()
{
    robot.setup();

    digitalWrite(N_SLEEP, LOW);
}

void loop()
{
    robot.update();
    delay(5);
}
