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
    SpeedConfig &getSpeedConfig();

    // Get sensor readings (for menu display or debugging)
    int *getIRValues();

    // Get display for menu screen rendering
    Display &getDisplay();

    // Get motor for sensor readings
    Motor &getMotor();

    // Menu and state management
    RobotMode getMode() const;
    void setMode(RobotMode mode);

    int getCurrentMenuScreen() const;
    void setCurrentMenuScreen(int screen);
    void cycleMenuScreen();

    bool isPaused() const;
    void togglePause();

    int getCurrentSpeedLevel() const;
    void setSpeedLevel(int level);
    void cycleSpeedLevel();

    int getCurrentStrategy() const;
    void setStrategy(int strategy);
    void cycleStrategy();

    int getCurrentDirection() const;

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
    unsigned long lastDecisionTime;                 // Time of last direction decision
    unsigned long lastTargetDetectionTime;          // Time when target was last detected
    static const unsigned long TURN_COMMIT_MS = 50; // Minimum time to commit to a turn (ms)

    // Decision-making: read sensors and command motors
    void updateBehavior();

    // Strategy implementations
    void updateBehavior_Speed();
    void updateBehavior_Run();

    // Apply speed preset to speed config
    void applySpeedPreset(int level);
};

#endif // ROBOT_H
