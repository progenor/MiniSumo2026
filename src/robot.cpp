#include "robot.h"

// Define global speed configuration (mutable at runtime)
SpeedConfig speedConfig;

Robot::Robot()
    : currentMode(MODE_MENU),
      currentMenuScreen(MENU_SCREEN_MAIN),
      paused(false),
      currentSpeedLevel(SPEED_LEVEL_LOW),
      currentStrategy(STRATEGY_STING),
      currentMotorDirection(DIRECTION_STOP)
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

    // Update motor PWM ramping (smooth acceleration to reduce current peaks)
    motor.updateMotorRamp();

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

// ===== SPEED STRATEGY =====
// Original strategy with all sensor detection
void Robot::updateBehavior_Speed()
{
    // Don't execute motor commands if paused
    if (paused)
    {
        motor.stop();
        currentMotorDirection = DIRECTION_STOP;
        return;
    }

    // Get current IR sensor readings
    int *irValues = irSensors.getAllValues();
    // Layout: [0]=LEFT, [1]=CENTER, [2]=RIGHT

    // ALL sensors detect target -> FULL SPEED PUSH
    if (irValues[0] == 1 && irValues[1] == 1 && irValues[2] == 1)
    {
        motor.forward(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_FORWARD;
    }
    // CENTER sensor detects -> DIRECT ATTACK
    else if (irValues[1] == 1)
    {
        motor.forward(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_FORWARD;
    }
    // LEFT sensor detects -> TURN LEFT + FORWARD (angled attack)
    else if (irValues[0] == 1)
    {
        motor.left(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_LEFT;
    }
    // RIGHT sensor detects -> TURN RIGHT + FORWARD (angled attack)
    else if (irValues[2] == 1)
    {
        motor.right(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_RIGHT;
    }
    // NO sensors detect -> SEARCH by spinning in place
    else
    {
        motor.right(speedConfig.search_speed);
        currentMotorDirection = DIRECTION_RIGHT;
    }
}
// ===== END SPEED STRATEGY =====

// ===== STING STRATEGY =====
// CENTER PRIORITY strategy - handles sensor intersections by prioritizing CENTER sensor
void Robot::updateBehavior_Sting()
{
    // Don't execute motor commands if paused
    if (paused)
    {
        motor.stop();
        currentMotorDirection = DIRECTION_STOP;
        return;
    }

    // Get current IR sensor readings
    int *irValues = irSensors.getAllValues();
    // Layout: [0]=LEFT, [1]=CENTER, [2]=RIGHT

    // CENTER sensor detected (regardless of LEFT/RIGHT) -> DIRECT ATTACK at full speed
    if (irValues[1] == 1)
    {
        motor.forward(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_FORWARD;
    }
    // LEFT sensor only (CENTER not detecting) -> TURN LEFT + FORWARD
    else if (irValues[0] == 1)
    {
        motor.left(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_LEFT;
    }
    // RIGHT sensor only (CENTER not detecting) -> TURN RIGHT + FORWARD
    else if (irValues[2] == 1)
    {
        motor.right(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_RIGHT;
    }
    // NO sensors detect -> SEARCH by spinning in place
    else
    {
        motor.right(speedConfig.search_speed);
        currentMotorDirection = DIRECTION_RIGHT;
    }
}
// ===== END STING STRATEGY =====

// ===== RUN STRATEGY =====
// Retreat/Reverse strategy - does the opposite
// If sensors detect, goes BACKWARD instead of forward
void Robot::updateBehavior_Run()
{
    // Don't execute motor commands if paused
    if (paused)
    {
        motor.stop();
        currentMotorDirection = DIRECTION_STOP;
        return;
    }

    // Get current IR sensor readings
    int *irValues = irSensors.getAllValues();
    // Layout: [0]=LEFT, [1]=CENTER, [2]=RIGHT

    // CENTER sensor detected -> RETREAT BACKWARD
    if (irValues[1] == 1)
    {
        motor.backward(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_BACKWARD;
    }
    // LEFT sensor only -> TURN RIGHT + BACKWARD (opposite of attack)
    else if (irValues[0] == 1)
    {
        motor.right(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_RIGHT;
    }
    // RIGHT sensor only -> TURN LEFT + BACKWARD (opposite of attack)
    else if (irValues[2] == 1)
    {
        motor.left(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_LEFT;
    }
    // NO sensors detect -> SEARCH by spinning backward
    else
    {
        motor.left(speedConfig.search_speed);
        currentMotorDirection = DIRECTION_LEFT;
    }
}
// ===== END RUN STRATEGY =====

// Strategy dispatcher
void Robot::updateBehavior()
{
    switch (currentStrategy)
    {
    case STRATEGY_STING:
        updateBehavior_Sting();
        break;
    case STRATEGY_SPEED:
        updateBehavior_Speed();
        break;
    case STRATEGY_RUN:
        updateBehavior_Run();
        break;
    default:
        updateBehavior_Sting();
        break;
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
        // Double press: cycle speed presets (only on speed screen) or strategies (on strategy screen)
        if (currentMenuScreen == MENU_SCREEN_SPEED)
        {
            cycleSpeedLevel();
        }
        else if (currentMenuScreen == MENU_SCREEN_STRATEGY)
        {
            cycleStrategy();
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

// ===== GETTER AND SETTER IMPLEMENTATIONS =====

SpeedConfig &Robot::getSpeedConfig()
{
    return speedConfig;
}

int *Robot::getIRValues()
{
    return irSensors.getAllValues();
}

Display &Robot::getDisplay()
{
    return display;
}

Motor &Robot::getMotor()
{
    return motor;
}

RobotMode Robot::getMode() const
{
    return currentMode;
}

void Robot::setMode(RobotMode mode)
{
    currentMode = mode;
}

int Robot::getCurrentMenuScreen() const
{
    return currentMenuScreen;
}

void Robot::setCurrentMenuScreen(int screen)
{
    currentMenuScreen = screen % MENU_SCREEN_COUNT;
}

void Robot::cycleMenuScreen()
{
    currentMenuScreen = (currentMenuScreen + 1) % MENU_SCREEN_COUNT;
}

bool Robot::isPaused() const
{
    return paused;
}

void Robot::togglePause()
{
    paused = !paused;
    if (paused)
    {
        motor.stop();
    }
}

int Robot::getCurrentSpeedLevel() const
{
    return currentSpeedLevel;
}

void Robot::setSpeedLevel(int level)
{
    if (level >= 0 && level < SPEED_LEVEL_COUNT)
    {
        currentSpeedLevel = level;
        applySpeedPreset(level);
    }
}

void Robot::cycleSpeedLevel()
{
    setSpeedLevel((currentSpeedLevel + 1) % SPEED_LEVEL_COUNT);
}

int Robot::getCurrentStrategy() const
{
    return currentStrategy;
}

void Robot::setStrategy(int strategy)
{
    if (strategy >= 0 && strategy < STRATEGY_COUNT)
    {
        currentStrategy = strategy;
    }
}

void Robot::cycleStrategy()
{
    setStrategy((currentStrategy + 1) % STRATEGY_COUNT);
}

int Robot::getCurrentDirection() const
{
    return currentMotorDirection;
}
