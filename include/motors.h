#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "pins.h"

class Motor
{
public:
    Motor();

    // Initialize motor pins and DRV8243 driver
    void setup();

    // Motor control methods
    void forward(int pwm = 128);
    void backward(int pwm = 128);
    void left(int pwm = 128);
    void right(int pwm = 128);
    void stop();

    // Current sensing methods
    float readMotorCurrent();  // Returns motor A current in Amps
    float readMotorBCurrent(); // Returns motor B current in Amps

    // Alpha-filtered current sensing methods (smooths noisy ADC readings)
    float getFilteredMotorCurrent();  // Returns alpha-filtered motor A current
    float getFilteredMotorBCurrent(); // Returns alpha-filtered motor B current

    // Peak current tracking methods
    void updatePeaks();           // Updates peak values based on current readings
    float getPeakMotorACurrent(); // Returns peak motor A current since last reset
    float getPeakMotorBCurrent(); // Returns peak motor B current since last reset
    float getTotalPeakCurrent();  // Returns sum of peak currents
    void resetPeaks();            // Resets peak values to zero

    // EEPROM storage methods
    void savePeaksToROM();   // Saves current peak values to flash memory (EEPROM)
    void loadPeaksFromROM(); // Loads peak values from flash memory
    void clearPeakROM();     // Clears saved peak values from ROM

private:
    // Current PWM values
    int pwm_A; // Current PWM for motor A
    int pwm_B; // Current PWM for motor B

    // Alpha filter configuration
    static const float ALPHA_FILTER; // Filter coefficient (0.97 - very strong smoothing)

    // Peak threshold configuration
    static const float PEAK_RESET_THRESHOLD; // Reset threshold in Amps (0.5A)

    // EEPROM storage address
    static const int EEPROM_ADDR_PEAK_A = 0; // Address for peak A current (4 bytes for float)
    static const int EEPROM_ADDR_PEAK_B = 4; // Address for peak B current (4 bytes for float)
    static const int EEPROM_SIZE = 8;        // Total bytes needed (2 floats)

    // Filter state variables
    float filteredCurrent_A; // Cached filtered value for motor A
    float filteredCurrent_B; // Cached filtered value for motor B
    bool isFirstRead_A;      // Flag to detect first read for motor A
    bool isFirstRead_B;      // Flag to detect first read for motor B

    // Peak tracking variables
    float peakCurrent_A; // Peak current for motor A
    float peakCurrent_B; // Peak current for motor B

    // Helper function to check and reset peaks exceeding threshold
    void checkAndResetHighPeaks();

    // DRV8243 initialization
    void initDRV8243();
};

#endif // MOTORS_H
