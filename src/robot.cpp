#include "robot.h"

// Define global speed configuration (mutable at runtime)
SpeedConfig speedConfig;

Robot::Robot()
{
}

void Robot::setup()
{
    // Initialize Serial for debugging
    Serial.begin(115200);
    delay(1000);

    // Initialize I2C for OLED
    Wire.begin();

    // Setup subsystems
    if (!display.setup())
    {
        // OLED failed, but continue with motor operation
        Serial.println("Warning: Display initialization failed");
    }
    delay(500);
    display.drawLoadingScreen();

    // Setup motor control
    motor.setup();

    // Setup IR sensors
    irSensors.setup();

    // Play startup melody
    playMelody();

    delay(1000);
}

void Robot::update()
{
    // Read all sensors
    irSensors.read();

    // Update behavior based on sensors
    updateBehavior();

    // Display sensor feedback
    display.displayIR(getIRValues(), IRCount);
}

void Robot::updateBehavior()
{
    // Get current IR sensor readings
    int *irValues = irSensors.getAllValues();

    // Obstacle avoidance logic (IR-based)
    // All three sensors detect obstacle -> move forward
    if (irValues[0] == 1 && irValues[1] == 1 && irValues[2] == 1)
    {
        motor.forward(speedConfig.search_speed);
    }
    // Left sensor detects obstacle -> turn right
    else if (irValues[0] == 0 && irValues[1] == 1 && irValues[2] == 1)
    {
        motor.right(speedConfig.turn_speed_moderate);
    }
    // Right sensor detects obstacle -> turn left
    else if (irValues[0] == 1 && irValues[1] == 1 && irValues[2] == 0)
    {
        motor.left(speedConfig.turn_speed_moderate);
    }
    // Default: move forward
    else
    {
        motor.forward(speedConfig.search_speed);
    }
}
