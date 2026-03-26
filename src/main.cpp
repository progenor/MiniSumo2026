
#include <Arduino.h>
#include "robot.h"

// Global robot instance
Robot robot;

void setup()
{
    robot.setup();
}

void loop()
{
    robot.update();
    delay(5);
}
