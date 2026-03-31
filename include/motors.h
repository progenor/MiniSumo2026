#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
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

    // PWM ramping methods (smooth acceleration to reduce current peaks)
    void updateMotorRamp();       // Call this from main loop to advance PWM ramping
    int getPWM_A_Current() const; // Returns current PWM value for motor A
    int getPWM_B_Current() const; // Returns current PWM value for motor B

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

private:
    // Alpha filter configuration
    static const float ALPHA_FILTER; // Filter coefficient (0.97 - very strong smoothing)

    // PWM ramping configuration
    static const int RAMP_DURATION_MS; // Ramp duration in milliseconds (300ms)
    static const int DELTA_THRESHOLD;  // Minimum PWM change to trigger ramp (70)

    // PWM ramping state variables
    unsigned long rampStartTime; // Timestamp when current ramp started (ms)
    int pwmA_current;            // Currently applied PWM for motor A
    int pwmA_target;             // Target PWM for motor A
    int pwmB_current;            // Currently applied PWM for motor B
    int pwmB_target;             // Target PWM for motor B
    bool isRamping;              // Whether a ramp is currently in progress

    // Filter state variables
    float filteredCurrent_A; // Cached filtered value for motor A
    float filteredCurrent_B; // Cached filtered value for motor B
    bool isFirstRead_A;      // Flag to detect first read for motor A
    bool isFirstRead_B;      // Flag to detect first read for motor B

    // Peak tracking variables
    float peakCurrent_A; // Peak current for motor A
    float peakCurrent_B; // Peak current for motor B

    // Helper method for ramping logic
    void applyRampedPWM(int targetA, int targetB); // Internal method to apply ramped PWM

    // DRV8243 initialization
    void initDRV8243();
};

#endif // MOTORS_H
