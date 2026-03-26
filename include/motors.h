#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
#include "pins.h"

class Motor
{
public:
    Motor();

    // Initialize motor pins and DRV8243 driver
    void setup();

    // Motor control methods
    void forward(int pwm = 128);
    void backward(int pwm = 128);
    void left(int pwm = 128);
    void right(int pwm = 128);
    void stop();

private:
    // DRV8243 initialization
    void initDRV8243();
};

#endif // MOTORS_H
