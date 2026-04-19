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
    int attack_speed; // Motor speed for direct attacks
    int search_speed; // Motor speed for searching (spin to find opponent)

    // Constructor with defaults
    SpeedConfig()
        : attack_speed(128),
          search_speed(64) {}
};

// Global speed configuration (can be modified at runtime)
extern SpeedConfig speedConfig;

#endif // DEFINES_H
