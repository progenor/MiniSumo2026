#include "robot.h"

// Define global speed configuration (mutable at runtime)
SpeedConfig speedConfig;

Robot::Robot()
    : currentMode(MODE_MENU),
      currentMenuScreen(MENU_SCREEN_IR),
      currentSpeedLevel(SPEED_LEVEL_LOW),
      currentStrategy(STRATEGY_ATTACK),
      currentMotorDirection(DIRECTION_STOP),
      modeStartTime(0)
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

    // Initialize startup timer for first battle
    modeStartTime = millis();

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

    // Log telemetry periodically (~every 500ms)
    static unsigned long last_telemetry_log = 0;
    if ((millis() - last_telemetry_log) > 500)
    {

        // Log IR sensor readings
        int *ir = irSensors.getAllValues();


        last_telemetry_log = millis();
    }
}

// ===== ATTACK STRATEGY =====
// Original strategy with all sensor detection
void Robot::updateBehavior_Speed()
{

    // Get current IR sensor readings
    int *irValues = irSensors.getAllValues();
    // Layout: [0]=LEFT, [1]=CENTER, [2]=RIGHT
    // CENTER sensor detects -> DIRECT ATTACK
    if (irValues[1] == 1)
    {
        if (currentMotorDirection != DIRECTION_FORWARD)
        {
            modeStartTime = millis(); // Reset startup timer only on direction change
        }
        motor.forward(getAttackSpeed());
        currentMotorDirection = DIRECTION_FORWARD;
    }
    // LEFT sensor detects -> TURN LEFT + FORWARD (angled attack)
    else if (irValues[0] == 1)
    {
        if (currentMotorDirection != DIRECTION_LEFT)
        {
            modeStartTime = millis(); // Reset startup timer only on direction change
        }
        motor.left(getAttackSpeed());
        currentMotorDirection = DIRECTION_LEFT;
    }
    // RIGHT sensor detects -> TURN RIGHT + FORWARD (angled attack)
    else if (irValues[2] == 1)
    {
        if (currentMotorDirection != DIRECTION_RIGHT)
        {
            modeStartTime = millis(); // Reset startup timer only on direction change
        }
        motor.right(getAttackSpeed());
        currentMotorDirection = DIRECTION_RIGHT;
    }
    // NO sensors detect -> SEARCH by spinning in place
    else
    {
        motor.right(speedConfig.search_speed); // Spin in place to search
        currentMotorDirection = DIRECTION_RIGHT;
    }
}
// ===== END ATTACK STRATEGY =====

// ===== RUN STRATEGY =====
// Retreat/Reverse strategy - does the opposite
// If sensors detect, goes BACKWARD instead of forward
void Robot::updateBehavior_Run()
{
    // Get current IR sensor readings
    int *irValues = irSensors.getAllValues();
    // Layout: [0]=LEFT, [1]=CENTER, [2]=RIGHT
    // CENTER sensor detected -> RETREAT BACKWARD
    if (irValues[1] == 1)
    {
        if (currentMotorDirection != DIRECTION_BACKWARD)
        {
            modeStartTime = millis(); // Reset startup timer only on direction change
        }

        motor.backward(getAttackSpeed());
        currentMotorDirection = DIRECTION_BACKWARD;
    }
    // RIGHT sensor only -> TURN RIGHT + BACKWARD (opposite of attack)
    else if (irValues[2] == 1)
    {
        if (currentMotorDirection != DIRECTION_RIGHT)
        {
            modeStartTime = millis(); // Reset startup timer only on direction change
        }
        motor.right(getAttackSpeed());
        currentMotorDirection = DIRECTION_RIGHT;
    }
    // LEFT sensor only -> TURN LEFT + BACKWARD (opposite of attack)
    else if (irValues[0] == 1)
    {
        if (currentMotorDirection != DIRECTION_LEFT)
        {
            modeStartTime = millis(); // Reset startup timer only on direction change
        }
        motor.left(getAttackSpeed());
        currentMotorDirection = DIRECTION_LEFT;
    }
    // NO sensors detect -> SEARCH by spinning backward
    else
    {
        currentMotorDirection = DIRECTION_STOP;
        motor.stop(); // Stop instead of spinning to create a more distinct behavior
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
    switch (gesture)
    {
    case GESTURE_SINGLE_PRESS:
        // Single click: cycle through menus (MAIN → IR → SPEED → STRATEGY → MAIN)
        cycleMenuScreen();
        break;

    case GESTURE_DOUBLE_PRESS:
        // Double click: toggle based on current menu screen
        {
            int activeScreen = ENABLED_SCREENS[getCurrentMenuScreen()];

            if (activeScreen == MENU_SCREEN_SPEED)
            {
                // On speed menu: cycle through speed levels (LOW → MEDIUM → HIGH → LOW)
                cycleSpeedLevel();
            }
            else if (activeScreen == MENU_SCREEN_STRATEGY)
            {
                // On strategy menu: cycle through strategies (ATTACK → RUN → ATTACK)
                cycleStrategy();
            }
        }
        break;

    case GESTURE_LONG_PRESS:
    case GESTURE_NONE:
    default:
        // No action for long press or none
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
        modeStartTime = millis(); // Reset startup timer when speed changes
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
        modeStartTime = millis(); // Reset startup timer when strategy changes
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

// ===== STARTUP SPEED HELPERS =====
// During the first 1 second of operation, use fixed reliable speeds
// After 1 second, switch to the selected speed preset
int Robot::getAttackSpeed()
{
    // If we're still in startup phase (first 1 second), use fixed speed
    if ((millis() - modeStartTime) < STARTUP_FIXED_SPEED_MS)
    {
        return 90; // Fixed startup attack speed
    }
    // After startup, use the preset speed
    return speedConfig.attack_speed;
}
