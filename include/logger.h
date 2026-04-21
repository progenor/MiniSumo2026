#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <EEPROM.h>

// Flash Memory Configuration (RP2040 internal flash)
#define FLASH_STORAGE_SIZE 4096  // 4KB for logging data
#define FLASH_STORAGE_START 0x00 // Start of EEPROM-like storage
#define MAX_FLASH_ENTRIES 20     // Max log entries in flash
#define FLASH_ENTRY_SIZE 128     // Bytes per entry

struct FlashLogEntry
{
    unsigned long timestamp_ms;
    char message[110]; // Smaller for flash storage
};

class Logger
{
public:
    Logger();

    // Initialize flash storage
    bool begin();

    // Log an event to flash (non-blocking)
    void log(const char *message);

    // Log motor telemetry: PWM and current values
    void logMotorTelemetry(int pwm_a, int pwm_b, float curr_a, float curr_b);

    // Log sensor data: IR values
    void logSensorData(int ir_left, int ir_center, int ir_right);

    // Log peak current spikes
    void logMotorPeaks(float peak_a, float peak_b);

    // Get status
    bool isReady() const { return flash_initialized; }

    // Flash storage methods
    void writeToFlash(const char *message);      // Write single entry to flash (non-blocking)
    void readFromFlash(int index, char *buffer); // Read entry from flash at index
    int getFlashEntryCount() const;              // Get number of entries in flash
    void clearFlash();                           // Clear all flash data
    void dumpFlash();                            // Print all flash data to Serial

private:
    bool flash_initialized;
    int flash_entry_count; // Current number of entries in flash

    // Flash helper methods
    void initFlash();               // Initialize flash storage
    int getFlashWriteIndex() const; // Get current write position in flash
};

extern Logger logger;

#endif // LOGGER_H
