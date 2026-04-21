#include "logger.h"
#include <stdio.h>
#include <string.h>

Logger logger;

// Flash memory layout:
// Bytes 0-3: flash_entry_count (uint32_t)
// Bytes 4+: Flash entries (128 bytes each)
#define FLASH_METADATA_SIZE 4
#define FLASH_DATA_START (FLASH_STORAGE_START + FLASH_METADATA_SIZE)

Logger::Logger()
    : flash_initialized(false),
      flash_entry_count(0)
{
}

bool Logger::begin()
{
    // Initialize EEPROM with default 4KB size
    EEPROM.begin(FLASH_STORAGE_SIZE);

    initFlash();
    flash_initialized = true;

    Serial.println("Flash storage initialized");
    return true;
}

void Logger::initFlash()
{
    // Read entry count from flash metadata
    uint32_t stored_count = 0;
    for (int i = 0; i < 4; i++)
    {
        stored_count |= ((uint32_t)EEPROM.read(FLASH_STORAGE_START + i)) << (i * 8);
    }

    // Validate and set entry count
    if (stored_count > MAX_FLASH_ENTRIES)
    {
        flash_entry_count = 0;
        clearFlash();
    }
    else
    {
        flash_entry_count = stored_count;
    }
}

int Logger::getFlashWriteIndex() const
{
    return flash_entry_count % MAX_FLASH_ENTRIES;
}

void Logger::log(const char *message)
{
    if (!flash_initialized)
        return;

    writeToFlash(message);
}

void Logger::logMotorTelemetry(int pwm_a, int pwm_b, float curr_a, float curr_b)
{
    if (!flash_initialized)
        return;

    // Format: "PWM:A=255,B=255 I:A=1.2,B=1.1" (compact for 110 char limit)
    char msg[110];
    snprintf(msg, sizeof(msg), "MOTOR: PWM_A=%d PWM_B=%d I_A=%.2f I_B=%.2f",
             pwm_a, pwm_b, curr_a, curr_b);
    writeToFlash(msg);
}

void Logger::logSensorData(int ir_left, int ir_center, int ir_right)
{
    if (!flash_initialized)
        return;

    // Format: "SENS: L=800 C=900 R=750"
    char msg[110];
    snprintf(msg, sizeof(msg), "SENS: L=%d C=%d R=%d", ir_left, ir_center, ir_right);
    writeToFlash(msg);
}

void Logger::logMotorPeaks(float peak_a, float peak_b)
{
    if (!flash_initialized)
        return;

    // Format: "PEAK: A=2.5 B=2.3"
    char msg[110];
    snprintf(msg, sizeof(msg), "PEAK_CURR: A=%.2f B=%.2f", peak_a, peak_b);
    writeToFlash(msg);
}

void Logger::writeToFlash(const char *message)
{
    if (!flash_initialized || !message)
        return;

    // Get write position
    int write_index = getFlashWriteIndex();
    int write_offset = FLASH_DATA_START + (write_index * FLASH_ENTRY_SIZE);

    // Prepare entry with timestamp
    FlashLogEntry entry;
    entry.timestamp_ms = millis();

    // Copy message (max 110 chars for flash)
    strncpy(entry.message, message, sizeof(entry.message) - 1);
    entry.message[sizeof(entry.message) - 1] = '\0';

    // Write timestamp (4 bytes)
    EEPROM.write(write_offset + 0, (entry.timestamp_ms >> 0) & 0xFF);
    EEPROM.write(write_offset + 1, (entry.timestamp_ms >> 8) & 0xFF);
    EEPROM.write(write_offset + 2, (entry.timestamp_ms >> 16) & 0xFF);
    EEPROM.write(write_offset + 3, (entry.timestamp_ms >> 24) & 0xFF);

    // Write message (110 bytes)
    int msg_len = strlen(entry.message);
    for (int i = 0; i < 110; i++)
    {
        if (i < msg_len)
            EEPROM.write(write_offset + 4 + i, entry.message[i]);
        else
            EEPROM.write(write_offset + 4 + i, 0); // Null padding
    }

    // Commit changes to EEPROM (non-blocking)
    EEPROM.commit();

    // Increment entry count if not wrapped yet
    if (flash_entry_count < MAX_FLASH_ENTRIES)
    {
        flash_entry_count++;

        // Update metadata
        EEPROM.write(FLASH_STORAGE_START + 0, (flash_entry_count >> 0) & 0xFF);
        EEPROM.write(FLASH_STORAGE_START + 1, (flash_entry_count >> 8) & 0xFF);
        EEPROM.write(FLASH_STORAGE_START + 2, (flash_entry_count >> 16) & 0xFF);
        EEPROM.write(FLASH_STORAGE_START + 3, (flash_entry_count >> 24) & 0xFF);
        EEPROM.commit();
    }
}

void Logger::readFromFlash(int index, char *buffer)
{
    if (!flash_initialized || !buffer || index < 0 || index >= flash_entry_count)
        return;

    int read_offset = FLASH_DATA_START + (index * FLASH_ENTRY_SIZE);

    // Read timestamp (4 bytes)
    uint32_t timestamp = 0;
    timestamp |= ((uint32_t)EEPROM.read(read_offset + 0)) << 0;
    timestamp |= ((uint32_t)EEPROM.read(read_offset + 1)) << 8;
    timestamp |= ((uint32_t)EEPROM.read(read_offset + 2)) << 16;
    timestamp |= ((uint32_t)EEPROM.read(read_offset + 3)) << 24;

    // Read message (110 bytes)
    char message[111];
    for (int i = 0; i < 110; i++)
    {
        message[i] = EEPROM.read(read_offset + 4 + i);
    }
    message[110] = '\0';

    // Format output with timestamp
    snprintf(buffer, 128, "[%lu] %s", timestamp, message);
}

int Logger::getFlashEntryCount() const
{
    return flash_entry_count;
}

void Logger::clearFlash()
{
    if (!flash_initialized)
        return;

    // Erase all flash data
    for (int i = FLASH_STORAGE_START; i < FLASH_STORAGE_SIZE; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();

    flash_entry_count = 0;
    Serial.println("Flash cleared");
}

void Logger::dumpFlash()
{
    if (!flash_initialized)
    {
        Serial.println("Flash not initialized");
        return;
    }

    Serial.printf("=== Flash Storage Dump ===\n");
    Serial.printf("Total entries: %d\n", flash_entry_count);
    Serial.println();

    char buffer[128];
    for (int i = 0; i < flash_entry_count; i++)
    {
        readFromFlash(i, buffer);
        Serial.printf("%d: %s\n", i, buffer);
    }

    Serial.println("=== End Dump ===\n");
}
