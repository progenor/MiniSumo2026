#ifndef MENU_H
#define MENU_H

// Menu screen indices
#define MENU_SCREEN_MAIN 0
#define MENU_SCREEN_SPEED 1
#define MENU_SCREEN_IR 2
#define MENU_SCREEN_STRATEGY 3
#define MENU_SCREEN_CURRENT 4
#define MENU_SCREEN_PEAK_CURRENT 5
#define MENU_SCREEN_COUNT 6     // Total available screens
#define ENABLED_SCREENS_COUNT 4 // Only 4 screens shown in menu cycle (see ENABLED_SCREENS)

// Robot mode enumeration
enum RobotMode
{
    MODE_MENU, // In menu navigation mode
};

// Speed level enumeration
enum SpeedLevel
{
    SPEED_LEVEL_LOW = 0,
    SPEED_LEVEL_MEDIUM = 1,
    SPEED_LEVEL_HIGH = 2,
    SPEED_LEVEL_COUNT = 3
};

// Speed preset values (for each level)
struct SpeedPreset
{
    uint16_t attack;
    uint16_t search;
};

// Preset configurations
static const SpeedPreset SPEED_PRESETS[SPEED_LEVEL_COUNT] = {
    // LOW
    {90, 60},
    // MEDIUM
    {200, 60},
    // HIGH
    {255, 60}};

// Strategy enumeration
enum Strategy
{
    STRATEGY_ATTACK = 0,
    STRATEGY_RUN = 1,
    STRATEGY_COUNT = 2
};

// ===== ENABLED SCREENS CONFIGURATION =====
// All 6 screens enabled for display in the menu
// Available screens: MAIN(0), SPEED(1), IR(2), STRATEGY(3), CURRENT(4), PEAK_CURRENT(5)

static const int ENABLED_SCREENS[ENABLED_SCREENS_COUNT] = {
    MENU_SCREEN_MAIN,
    MENU_SCREEN_SPEED,
    MENU_SCREEN_IR,
    MENU_SCREEN_STRATEGY,
};

// Motor direction enumeration
enum MotorDirection
{
    DIRECTION_FORWARD = 0,
    DIRECTION_BACKWARD = 1,
    DIRECTION_LEFT = 2,
    DIRECTION_RIGHT = 3,
    DIRECTION_STOP = 4
};

#endif // MENU_H
