
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

    // Initialize button input
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
        // Display menu screens based on currentMenuScreen
        int currentScreen = robot.getCurrentMenuScreen();

        switch (currentScreen)
        {
        case MENU_SCREEN_SPEED:
            robot.getDisplay().drawSpeedSelectorScreen(robot.getCurrentSpeedLevel());
            break;

        case MENU_SCREEN_SETTINGS:
            robot.getDisplay().drawSettingsScreen("ENABLED", "N/A", "N/A");
            break;

        case MENU_SCREEN_SENSORS:
            robot.getDisplay().drawSensorReadingsScreen(robot.getIRValues(), IRCount);
            break;

        case MENU_SCREEN_STATUS:
            robot.getDisplay().drawStatusScreen(robot.getMode(), robot.isPaused(), currentScreen);
            break;

        default:
            robot.getDisplay().drawStatusScreen(robot.getMode(), robot.isPaused(), currentScreen);
            break;
        }
    }
    // MODE_RUNNING and MODE_PAUSED display handled in robot.update()

    delay(5);
}
