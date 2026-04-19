
#include <Arduino.h>
#include "robot.h"
#include "button.h"
#include "menu.h"

// Global robot instance
Robot robot;

// Global button manager instance
ButtonManager buttonManager;

void setup()
{
    robot.setup();
    buttonManager.setup();
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

    delay(5);
}
