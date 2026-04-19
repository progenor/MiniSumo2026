#include "logger.h"
#include <stdio.h>
#include <stdarg.h>

Logger logger;

Logger::Logger()
    : buffer_index(0),
      last_flush_time(0),
      sd_initialized(false),
      boot_count(0)
{
}

bool Logger::begin()
{
    // Initialize SPI for SD card (RP2040 SPI format)
    // SD.begin() handles SPI setup automatically with default pins
    // If SD card is on non-default pins, use: SD.begin(chipSelect, SPIBusSpeed)

    if (!SD.begin(SD_CHIP_SELECT))
    {
        Serial.println("SD Card initialization failed!");
        return false;
    }

    sd_initialized = true;
    Serial.println("SD Card initialized successfully");

    // Read existing boot count
    readBootCount();
    incrementBootCount();

    return true;
}

void Logger::log(const char *event_name, const char *details)
{
    if (!sd_initialized)
        return;

    if (buffer_index >= MAX_LOG_BUFFER)
    {
        flush();
    }

    buffer[buffer_index].timestamp_ms = millis();
    snprintf(buffer[buffer_index].message, sizeof(buffer[buffer_index].message),
             "[%lu] %s: %s", buffer[buffer_index].timestamp_ms, event_name, details);
    buffer_index++;

    // Auto-flush based on time interval
    if ((millis() - last_flush_time) > LOG_BATCH_INTERVAL_MS && buffer_index > 0)
    {
        flush();
    }
}

void Logger::logf(const char *event_name, const char *format, ...)
{
    if (!sd_initialized)
        return;

    if (buffer_index >= MAX_LOG_BUFFER)
    {
        flush();
    }

    char details[150];
    va_list args;
    va_start(args, format);
    vsnprintf(details, sizeof(details), format, args);
    va_end(args);

    buffer[buffer_index].timestamp_ms = millis();
    snprintf(buffer[buffer_index].message, sizeof(buffer[buffer_index].message),
             "[%lu] %s: %s", buffer[buffer_index].timestamp_ms, event_name, details);
    buffer_index++;

    // Auto-flush based on time interval
    if ((millis() - last_flush_time) > LOG_BATCH_INTERVAL_MS && buffer_index > 0)
    {
        flush();
    }
}

void Logger::logTelemetry(int pwm_a, int pwm_b, float current_a, float current_b, unsigned int loop_ms)
{
    if (!sd_initialized)
        return;

    if (buffer_index >= MAX_LOG_BUFFER)
    {
        flush();
    }

    buffer[buffer_index].timestamp_ms = millis();
    snprintf(buffer[buffer_index].message, sizeof(buffer[buffer_index].message),
             "[%lu] TELEMETRY: pwm_a=%d, pwm_b=%d, current_a=%.2f, current_b=%.2f, loop_ms=%u",
             buffer[buffer_index].timestamp_ms, pwm_a, pwm_b, current_a, current_b, loop_ms);
    buffer_index++;

    // Auto-flush based on time interval
    if ((millis() - last_flush_time) > LOG_BATCH_INTERVAL_MS && buffer_index > 0)
    {
        flush();
    }
}

void Logger::logCurrentSpike(float peak_a, float peak_b)
{
    if (!sd_initialized)
        return;

    if (buffer_index >= MAX_LOG_BUFFER)
    {
        flush();
    }

    buffer[buffer_index].timestamp_ms = millis();
    snprintf(buffer[buffer_index].message, sizeof(buffer[buffer_index].message),
             "[%lu] CURRENT_SPIKE: peak_a=%.2f, peak_b=%.2f", buffer[buffer_index].timestamp_ms, peak_a, peak_b);
    buffer_index++;
}

void Logger::logBootEvent(uint32_t boot_count)
{
    if (!sd_initialized)
        return;

    if (buffer_index >= MAX_LOG_BUFFER)
    {
        flush();
    }

    buffer[buffer_index].timestamp_ms = millis();
    snprintf(buffer[buffer_index].message, sizeof(buffer[buffer_index].message),
             "[%lu] BOOT: boot_count=%lu, uptime_start=0", buffer[buffer_index].timestamp_ms, boot_count);
    buffer_index++;

    flush(); // Flush immediately on boot for critical event
}

void Logger::flush()
{
    if (!sd_initialized || buffer_index == 0)
        return;

    File logfile = SD.open("/battle_log.txt", FILE_WRITE);
    if (!logfile)
    {
        Serial.println("Failed to open log file");
        return;
    }

    // Write all buffered entries
    for (int i = 0; i < buffer_index; i++)
    {
        logfile.println(buffer[i].message);
    }

    logfile.close();
    buffer_index = 0;
    last_flush_time = millis();

    Serial.println("Log flushed to SD card");
}

uint32_t Logger::getBootCount()
{
    return boot_count;
}

void Logger::incrementBootCount()
{
    boot_count++;
    saveBootCount();
}

void Logger::readBootCount()
{
    File config_file = SD.open("/config.txt", FILE_READ);
    if (!config_file)
    {
        boot_count = 0;
        return;
    }

    char line[50];
    if (config_file.available())
    {
        int len = config_file.readBytesUntil('\n', line, sizeof(line) - 1);
        line[len] = '\0';
        sscanf(line, "boot_count=%lu", &boot_count);
    }

    config_file.close();
}

void Logger::saveBootCount()
{
    File config_file = SD.open("/config.txt", FILE_WRITE);
    if (!config_file)
    {
        Serial.println("Failed to open config file");
        return;
    }

    // Truncate and write new value
    config_file.print("boot_count=");
    config_file.println(boot_count);
    config_file.close();
}
