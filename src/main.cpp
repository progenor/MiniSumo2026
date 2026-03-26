
#include <Arduino.h>
#include "robot.h"

// Global robot instance
Robot robot;

void setup()
{
    robot.setup();

    // IMPORTANT: PUT TO HIGH if you want to use motors
    digitalWrite(N_SLEEP, LOW);
}

void loop()
{
    robot.update();
    delay(5);
}
