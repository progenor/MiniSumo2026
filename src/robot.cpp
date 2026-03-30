#include "robot.h"

// Define global speed configuration (mutable at runtime)
SpeedConfig speedConfig;

Robot::Robot()
    : currentMode(MODE_MENU),
      currentMenuScreen(MENU_SCREEN_MAIN),
      paused(false),
      currentSpeedLevel(SPEED_LEVEL_LOW)
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

    // Update motor peak current tracking
    motor.updatePeaks();

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

// ===== OLD VERSION v1 - COMMENTED OUT =====
// This was the original strategy that caused jitter at sensor intersections
// Kept for reference/debugging
/*
void Robot::updateBehavior_OLD_v1()
{
    // Don't execute motor commands if paused
    if (paused)
    {
        motor.stop();
        return;
    }

    // Get current IR sensor readings
    int *irValues = irSensors.getAllValues();
    // Layout: [0]=LEFT, [1]=CENTER, [2]=RIGHT

    // ALL sensors detect target -> FULL SPEED PUSH
    if (irValues[0] == 1 && irValues[1] == 1 && irValues[2] == 1)
    {
        motor.forward(speedConfig.attack_speed);
    }
    // CENTER sensor detects -> DIRECT ATTACK
    else if (irValues[1] == 1)
    {
        motor.forward(speedConfig.attack_speed);
    }
    // LEFT sensor detects -> TURN LEFT + FORWARD (angled attack)
    else if (irValues[0] == 1)
    {
        motor.left(speedConfig.attack_speed);
    }
    // RIGHT sensor detects -> TURN RIGHT + FORWARD (angled attack)
    else if (irValues[2] == 1)
    {
        motor.right(speedConfig.attack_speed);
    }
    // NO sensors detect -> SEARCH by spinning in place
    else
    {
        motor.right(speedConfig.search_speed);
    }
}
*/
// ===== END OLD VERSION v1 =====

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
    // Layout: [0]=LEFT, [1]=CENTER, [2]=RIGHT

    // NEW VERSION v2 - Handles sensor intersection overlaps
    // The left/center and right/center sensors sometimes overlap due to physical placement
    // Using reduced PWM for turns when intersections occur prevents jitter

    // ALL sensors detect target -> FULL SPEED PUSH
    if (irValues[0] == 1 && irValues[1] == 1 && irValues[2] == 1)
    {
        motor.forward(speedConfig.attack_speed);
    }
    // LEFT + CENTER intersection -> GENTLE LEFT TURN (reduced speed to prevent jitter)
    else if (irValues[0] == 1 && irValues[1] == 1)
    {
        motor.left(speedConfig.turn_speed_moderate);
    }
    // RIGHT + CENTER intersection -> GENTLE RIGHT TURN (reduced speed to prevent jitter)
    else if (irValues[2] == 1 && irValues[1] == 1)
    {
        motor.right(speedConfig.turn_speed_moderate);
    }
    // CENTER sensor only -> DIRECT ATTACK
    else if (irValues[1] == 1)
    {
        motor.forward(speedConfig.attack_speed);
    }
    // LEFT sensor only -> AGGRESSIVE LEFT TURN
    else if (irValues[0] == 1)
    {
        motor.left(speedConfig.attack_speed);
    }
    // RIGHT sensor only -> AGGRESSIVE RIGHT TURN
    else if (irValues[2] == 1)
    {
        motor.right(speedConfig.attack_speed);
    }
    // NO sensors detect -> SEARCH by spinning in place
    else
    {
        motor.right(speedConfig.search_speed);
    }
}

void Robot::handleButtonGesture(ButtonGesture gesture)
{
    switch (gesture)
    {
    case GESTURE_SINGLE_PRESS:
        // Single press: cycle menu screens (stay in menu)
        if (currentMode == MODE_MENU)
        {
            cycleMenuScreen();
        }
        break;

    case GESTURE_DOUBLE_PRESS:
        // Double press: cycle speed presets (only on speed screen)
        if (currentMenuScreen == MENU_SCREEN_SPEED)
        {
            cycleSpeedLevel();
        }
        break;

    case GESTURE_LONG_PRESS:
        // Long press: toggle pause/resume (works from any mode/screen)
        togglePause();
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
