#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

// SD Card Pins (PlatformIO RP2040 SPI configuration)
// MISO = GPIO 4,  MOSI = GPIO 7, SCK = GPIO 6, CS = GPIO 5
#define SD_CHIP_SELECT 5

// Event log buffer
#define MAX_LOG_BUFFER 50          // Write to SD every 50 events
#define LOG_BATCH_INTERVAL_MS 5000 // Or every 5 seconds

struct LogEntry
{
    unsigned long timestamp_ms;
    char message[200]; // Event description + values
};

class Logger
{
public:
    Logger();

    // Initialize SD card
    bool begin();

    // Log an event to buffer
    void log(const char *event_name, const char *details);

    // Log with formatted values: "event_name: key1=value1, key2=value2"
    void logf(const char *event_name, const char *format, ...);

    // Log motor telemetry: timestamp, pwm_A, pwm_B, current_A, current_B, loop_time_ms
    void logTelemetry(int pwm_a, int pwm_b, float current_a, float current_b, unsigned int loop_ms);

    // Log peak current spike warning
    void logCurrentSpike(float peak_a, float peak_b);

    // Log reset event
    void logBootEvent(uint32_t boot_count);

    // Flush buffer to SD card
    void flush();

    // Get boot count from config file
    uint32_t getBootCount();

    // Increment and save boot count
    void incrementBootCount();

    // Get status
    bool isReady() const { return sd_initialized; }

private:
    LogEntry buffer[MAX_LOG_BUFFER];
    int buffer_index;
    unsigned long last_flush_time;
    bool sd_initialized;
    uint32_t boot_count;

    // Write buffer contents to SD
    void writeToSD();

    // Read boot count from config file
    void readBootCount();

    // Write boot count to config file
    void saveBootCount();
};

extern Logger logger;

#endif // LOGGER_H
