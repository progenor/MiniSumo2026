#ifndef DEFINES_H
#define DEFINES_H

// --- Display Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define display_RESET -1
#define DISPLAY_REFRESH_MS 30 // Display refresh throttle (milliseconds) ~33 FPS

// --- Speed Configuration Structure ---
// These are runtime-mutable values, can be changed from menu
struct SpeedConfig
{
    int attack_speed;
    int search_speed;
    int turn_speed_aggressive;
    int turn_speed_moderate;
    int turn_speed_gentle;

    // Constructor with defaults
    SpeedConfig()
        : attack_speed(250),
          search_speed(128),
          turn_speed_aggressive(50),
          turn_speed_moderate(30),
          turn_speed_gentle(15) {}
};

// Global speed configuration (can be modified at runtime)
extern SpeedConfig speedConfig;

#endif // DEFINES_H
