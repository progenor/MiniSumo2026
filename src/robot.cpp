#include "robot.h"

// Define global speed configuration (mutable at runtime)
SpeedConfig speedConfig;

Robot::Robot()
    : currentMode(MODE_MENU),
      currentMenuScreen(MENU_SCREEN_IR),
      paused(false),
      currentSpeedLevel(SPEED_LEVEL_LOW),
      currentStrategy(STRATEGY_ATTACK),
      currentMotorDirection(DIRECTION_STOP),
      lastDecisionTime(0),
      lastTargetDetectionTime(0)
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

    // Autonomous behavior: read sensors and command motors
    updateBehavior();

    // Display IR sensor readings
    display.displayIR(getIRValues(), IRCount);
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

    // Check if still in turn commitment window
    bool inCommitmentWindow = (millis() - lastDecisionTime) < TURN_COMMIT_MS;

    // If in commitment window, maintain current direction without re-evaluating sensors
    if (inCommitmentWindow && currentMotorDirection != DIRECTION_STOP)
    {
        if (currentMotorDirection == DIRECTION_FORWARD)
        {
            motor.forward(speedConfig.attack_speed);
        }
        else if (currentMotorDirection == DIRECTION_LEFT)
        {
            motor.left(speedConfig.attack_speed);
        }
        else if (currentMotorDirection == DIRECTION_RIGHT)
        {
            motor.right(speedConfig.attack_speed);
        }
        return;
    }

    // Outside commitment window: evaluate sensors fresh
    // CENTER sensor detects -> DIRECT ATTACK
    if (irValues[1] == 1)
    {
        motor.forward(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_FORWARD;
        lastDecisionTime = millis();
    }
    // LEFT sensor detects -> TURN LEFT + FORWARD (angled attack)
    else if (irValues[0] == 1)
    {
        motor.left(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_LEFT;
        lastDecisionTime = millis();
    }
    // RIGHT sensor detects -> TURN RIGHT + FORWARD (angled attack)
    else if (irValues[2] == 1)
    {
        motor.right(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_RIGHT;
        lastDecisionTime = millis();
    }
    // NO sensors detect -> SEARCH by spinning in place
    else
    {
        motor.right(speedConfig.search_speed);
        currentMotorDirection = DIRECTION_RIGHT;
    }
}
// ===== END SPEED STRATEGY =====

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

    // Check if still in turn commitment window
    bool inCommitmentWindow = (millis() - lastDecisionTime) < TURN_COMMIT_MS;

    // If in commitment window, maintain current direction without re-evaluating sensors
    if (inCommitmentWindow && currentMotorDirection != DIRECTION_STOP)
    {
        if (currentMotorDirection == DIRECTION_BACKWARD)
        {
            motor.backward(speedConfig.attack_speed);
        }
        else if (currentMotorDirection == DIRECTION_LEFT)
        {
            motor.left(speedConfig.attack_speed);
        }
        else if (currentMotorDirection == DIRECTION_RIGHT)
        {
            motor.right(speedConfig.attack_speed);
        }
        return;
    }

    // Outside commitment window: evaluate sensors fresh
    // CENTER sensor detected -> RETREAT BACKWARD
    if (irValues[1] == 1)
    {
        motor.backward(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_BACKWARD;
        lastDecisionTime = millis();
    }
    // RIGHT sensor only -> TURN RIGHT + BACKWARD (opposite of attack)
    else if (irValues[2] == 1)
    {
        motor.right(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_RIGHT;
        lastDecisionTime = millis();
    }
    // LEFT sensor only -> TURN LEFT + BACKWARD (opposite of attack)
    else if (irValues[0] == 1)
    {
        motor.left(speedConfig.attack_speed);
        currentMotorDirection = DIRECTION_LEFT;
        lastDecisionTime = millis();
    }
    // NO sensors detect -> SEARCH by spinning backward
    else
    {
        currentMotorDirection = DIRECTION_STOP;
        motor.stop();                // Stop instead of spinning to create a more distinct behavior
        lastDecisionTime = millis(); // Reset decision timer to avoid immediate re-evaluation
    }
}
// ===== END RUN STRATEGY =====

// Strategy dispatcher
void Robot::updateBehavior()
{
    switch (currentStrategy)
    {
    case STRATEGY_ATTACK:
        updateBehavior_Speed();
        break;
    case STRATEGY_RUN:
        updateBehavior_Run();
        break;
    default:
        updateBehavior_Speed();
        break;
    }
}

void Robot::handleButtonGesture(ButtonGesture gesture)
{
    // Button gestures currently not used in autonomous-only mode
    // Hardware start module handles motor driver enable
    switch (gesture)
    {
    case GESTURE_DOUBLE_PRESS:
        // Could use for real-time strategy toggle if needed
        // cycleStrategy();
        break;

    case GESTURE_SINGLE_PRESS:
    case GESTURE_LONG_PRESS:
    case GESTURE_NONE:
    default:
        // No action
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
    currentMenuScreen = screen % ENABLED_SCREENS_COUNT;
}

void Robot::cycleMenuScreen()
{
    currentMenuScreen = (currentMenuScreen + 1) % ENABLED_SCREENS_COUNT;
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
