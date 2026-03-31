#ifndef MENU_H
#define MENU_H

// Menu screen indices
#define MENU_SCREEN_MAIN 0
#define MENU_SCREEN_SPEED 1
#define MENU_SCREEN_IR 2
#define MENU_SCREEN_STRATEGY 3
#define MENU_SCREEN_CURRENT 4
#define MENU_SCREEN_PEAK_CURRENT 5
#define MENU_SCREEN_DIRECTION 6
#define MENU_SCREEN_COUNT 7

// Robot mode enumeration
enum RobotMode
{
    MODE_MENU,    // In menu navigation mode
    MODE_RUNNING, // Running normally (IR behavior active)
    MODE_PAUSED   // Paused (sensors active, motors stopped)
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
    uint16_t turn_moderate;
    uint16_t turn_gentle;
};

// Preset configurations
static const SpeedPreset SPEED_PRESETS[SPEED_LEVEL_COUNT] = {
    // LOW
    {150, 80, 20, 10},
    // MEDIUM
    {200, 120, 25, 15},
    // HIGH
    {255, 180, 40, 25}};

// Strategy enumeration
enum Strategy
{
    STRATEGY_STING = 0,
    STRATEGY_SPEED = 1,
    STRATEGY_RUN = 2,
    STRATEGY_COUNT = 3
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
