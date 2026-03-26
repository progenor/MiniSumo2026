#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include "motors.h"
#include "display.h"
#include "ir.h"
#include "melody.h"
#include "defines.h"

class Robot
{
public:
    Robot();

    // Initialize all subsystems
    void setup();

    // Main control loop
    void update();

    // Allow external menu to update speed config
    SpeedConfig &getSpeedConfig() { return speedConfig; }

    // Get sensor readings (for menu display or debugging)
    int *getIRValues() { return irSensors.getAllValues(); }

private:
    Motor motor;
    Display display;
    IRSensors irSensors;

    // Decision-making: read sensors and command motors
    void updateBehavior();
};

#endif // ROBOT_H
