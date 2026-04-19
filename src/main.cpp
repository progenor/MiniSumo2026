
#include <Arduino.h>
#include "robot.h"
#include "button.h"
#include "menu.h"
#include "logger.h"

// Global robot instance
Robot robot;

// Global button manager instance
ButtonManager buttonManager;

void setup()
{
    robot.setup();
    buttonManager.setup();

    // Initialize SD card logger
    if (!logger.begin())
    {
        Serial.println("Warning: SD card logger not available");
    }
    else
    {
        logger.logBootEvent(logger.getBootCount());
        Serial.print("Boot count: ");
        Serial.println(logger.getBootCount());
    }
}

void loop()
{
    // Update button state and detect gestures
    buttonManager.update();
    ButtonGesture gesture = buttonManager.getGesture();

    // Handle button input
    if (gesture != GESTURE_NONE)
    {
        robot.handleButtonGesture(gesture);
    }

    // Update robot sensors and behavior
    robot.update();

    // Display appropriate screen based on mode
    RobotMode currentMode = robot.getMode();

    if (currentMode == MODE_MENU)
    {
        // Display menu screens based on currentMenuScreen - map through ENABLED_SCREENS array
        int currentScreen = ENABLED_SCREENS[robot.getCurrentMenuScreen()];

        switch (currentScreen)
        {
        case MENU_SCREEN_MAIN:
            robot.getDisplay().drawMainScreen();
            break;

        case MENU_SCREEN_SPEED:
            robot.getDisplay().drawSpeedSelectorScreen(robot.getCurrentSpeedLevel());
            break;

        case MENU_SCREEN_CURRENT:
        {
            // Format filtered current readings as strings
            char motorA_str[10], motorB_str[10];
            snprintf(motorA_str, sizeof(motorA_str), "%.2f", robot.getMotor().getFilteredMotorCurrent());
            snprintf(motorB_str, sizeof(motorB_str), "%.2f", robot.getMotor().getFilteredMotorBCurrent());
            robot.getDisplay().drawCurentReading(motorA_str, motorB_str);
            break;
        }

        case MENU_SCREEN_PEAK_CURRENT:
        {
            // Format peak current readings as strings
            char peakA_str[10], peakB_str[10], peakTotal_str[10];
            snprintf(peakA_str, sizeof(peakA_str), "%.2f", robot.getMotor().getPeakMotorACurrent());
            snprintf(peakB_str, sizeof(peakB_str), "%.2f", robot.getMotor().getPeakMotorBCurrent());
            snprintf(peakTotal_str, sizeof(peakTotal_str), "%.2f", robot.getMotor().getTotalPeakCurrent());
            robot.getDisplay().drawPEAK_Current(peakA_str, peakB_str, peakTotal_str);
            break;
        }

        case MENU_SCREEN_IR:
            robot.getDisplay().displayIR(robot.getIRValues(), IRCount);
            break;

        case MENU_SCREEN_STRATEGY:
            robot.getDisplay().drawStrategySelectorScreen(robot.getCurrentStrategy());
            break;

        default:
            robot.getDisplay().drawMainScreen();
            break;
        }
    }

    // Optional: Log telemetry periodically (every ~1 second)
    static unsigned long last_telemetry_log = 0;
    if (logger.isReady() && (millis() - last_telemetry_log) > 1000)
    {
        // Log current motor telemetry for diagnostics
        // This helps identify power issues, current spikes, etc.
        logger.logTelemetry(0, 0, robot.getMotor().getFilteredMotorCurrent(),
                            robot.getMotor().getFilteredMotorBCurrent(), 5);
        last_telemetry_log = millis();

        // Log high current warnings
        if (robot.getMotor().getTotalPeakCurrent() > 1.2)
        {
            logger.logCurrentSpike(robot.getMotor().getPeakMotorACurrent(),
                                   robot.getMotor().getPeakMotorBCurrent());
        }
    }

    delay(5);
}
