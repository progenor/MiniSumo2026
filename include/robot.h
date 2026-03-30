#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include "motors.h"
#include "display.h"
#include "ir.h"
#include "melody.h"
#include "defines.h"
#include "menu.h"
#include "button.h"

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

    // Get display for menu screen rendering
    Display &getDisplay() { return display; }

    // Get motor for sensor readings
    Motor &getMotor() { return motor; }

    // Menu and state management
    RobotMode getMode() const { return currentMode; }
    void setMode(RobotMode mode) { currentMode = mode; }

    int getCurrentMenuScreen() const { return currentMenuScreen; }
    void setCurrentMenuScreen(int screen)
    {
        currentMenuScreen = screen % MENU_SCREEN_COUNT;
    }
    void cycleMenuScreen()
    {
        currentMenuScreen = (currentMenuScreen + 1) % MENU_SCREEN_COUNT;
    }

    bool isPaused() const { return paused; }
    void togglePause()
    {
        paused = !paused;
        if (paused)
        {
            motor.stop();
        }
    }

    int getCurrentSpeedLevel() const { return currentSpeedLevel; }
    void setSpeedLevel(int level)
    {
        if (level >= 0 && level < SPEED_LEVEL_COUNT)
        {
            currentSpeedLevel = level;
            applySpeedPreset(level);
        }
    }
    void cycleSpeedLevel()
    {
        setSpeedLevel((currentSpeedLevel + 1) % SPEED_LEVEL_COUNT);
    }

    int getCurrentStrategy() const { return currentStrategy; }
    void setStrategy(int strategy)
    {
        if (strategy >= 0 && strategy < STRATEGY_COUNT)
        {
            currentStrategy = strategy;
        }
    }
    void cycleStrategy()
    {
        setStrategy((currentStrategy + 1) % STRATEGY_COUNT);
    }

    int getCurrentDirection() const { return currentMotorDirection; }

    // Handle button gesture input
    void handleButtonGesture(ButtonGesture gesture);

private:
    Motor motor;
    Display display;
    IRSensors irSensors;

    // Menu state
    RobotMode currentMode;
    int currentMenuScreen;
    bool paused;
    int currentSpeedLevel;
    int currentStrategy;
    int currentMotorDirection;

    // Decision-making: read sensors and command motors
    void updateBehavior();

    // Strategy implementations
    void updateBehavior_Speed();
    void updateBehavior_Sting();
    void updateBehavior_Run();

    // Apply speed preset to speed config
    void applySpeedPreset(int level);
};

#endif // ROBOT_H
