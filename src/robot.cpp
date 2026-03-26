#include "robot.h"

// Define global speed configuration (mutable at runtime)
SpeedConfig speedConfig;

Robot::Robot()
    : currentMode(MODE_MENU),
      currentMenuScreen(MENU_SCREEN_SPEED),
      paused(false),
      currentSpeedLevel(SPEED_LEVEL_MEDIUM)
{
}

void Robot::setup()
{
    // Initialize Serial for debugging
    Serial.begin(115200);
    delay(1000);

    // Initialize I2C for display
    Wire.begin();

    // Setup subsystems
    if (!display.setup())
    {
        // display failed, but continue with motor operation
        Serial.println("Warning: Display initialization failed");
    }
    delay(500);
    display.drawLoadingScreen();

    // Setup motor control
    motor.setup();

    // Setup IR sensors
    irSensors.setup();

    // Setup button input
    ButtonManager buttonManager;
    buttonManager.setup();

    // Apply default speed preset
    applySpeedPreset(currentSpeedLevel);

    // Play startup melody
    playMelody();

    delay(1000);
}

void Robot::update()
{
    // Read all sensors
    irSensors.read();

    // Handle based on current mode
    if (currentMode == MODE_MENU)
    {
        // Update behavior and display menu screens
        updateBehavior();
        // Display will be handled in main.cpp based on currentMenuScreen
    }
    else if (currentMode == MODE_PAUSED)
    {
        // Paused mode: update behavior but motors are stopped
        updateBehavior();
        display.displayIR(getIRValues(), IRCount);
    }
    else
    { // MODE_RUNNING
        // Normal operation
        updateBehavior();
        display.displayIR(getIRValues(), IRCount);
    }
}

void Robot::updateBehavior()
{
    // Don't execute motor commands if paused
    if (paused)
    {
        motor.stop();
        return;
    }

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

void Robot::handleButtonGesture(ButtonGesture gesture)
{
    switch (gesture)
    {
    case GESTURE_SINGLE_PRESS:
        // Single press: cycle menu screens
        if (currentMode == MODE_MENU)
        {
            cycleMenuScreen();
        }
        else if (currentMode == MODE_RUNNING || currentMode == MODE_PAUSED)
        {
            // Single press while running: return to menu
            currentMode = MODE_MENU;
            currentMenuScreen = MENU_SCREEN_STATUS;
            paused = false;
        }
        break;

    case GESTURE_DOUBLE_PRESS:
        // Double press: cycle speed presets
        cycleSpeedLevel();
        break;

    case GESTURE_LONG_PRESS:
        // Long press: toggle pause/resume
        if (currentMode == MODE_MENU)
        {
            // Start running if in menu
            currentMode = MODE_RUNNING;
            paused = false;
        }
        else if (currentMode == MODE_RUNNING)
        {
            // Pause if running
            currentMode = MODE_PAUSED;
            togglePause();
        }
        else if (currentMode == MODE_PAUSED)
        {
            // Resume if paused
            currentMode = MODE_RUNNING;
            paused = false;
        }
        break;

    case GESTURE_NONE:
    default:
        // No gesture detected
        break;
    }
}

void Robot::applySpeedPreset(int level)
{
    if (level >= 0 && level < SPEED_LEVEL_COUNT)
    {
        const SpeedPreset &preset = SPEED_PRESETS[level];
        speedConfig.attack_speed = preset.attack;
        speedConfig.search_speed = preset.search;
        speedConfig.turn_speed_moderate = preset.turn_moderate;
        speedConfig.turn_speed_gentle = preset.turn_gentle;
    }
}
