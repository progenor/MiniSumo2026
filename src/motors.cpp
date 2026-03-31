#include "motors.h"
#include "pins.h"

// Current sensing constants (IPROPI configuration)
const float R_IPROPI = 1000.0; // Resistor value in Ohms (1kΩ)
const float A_IPROPI = 3075.0; // Datasheet scaling factor
const float V_REF = 3.3;       // Pico reference voltage

// Alpha filter coefficient for current smoothing
const float Motor::ALPHA_FILTER = 0.97; // 3% new value, 97% previous (very strong smoothing/low-pass filter)

// PWM ramping configuration
const int Motor::RAMP_DURATION_MS = 300; // Smooth acceleration over 300ms
const int Motor::DELTA_THRESHOLD = 40;   // Only ramp if PWM change exceeds 70

Motor::Motor()
{
    // Constructor - pins will be initialized in setup()
    filteredCurrent_A = 0.0f;
    filteredCurrent_B = 0.0f;
    isFirstRead_A = true;
    isFirstRead_B = true;
    peakCurrent_A = 0.0f;
    peakCurrent_B = 0.0f;

    // Initialize PWM ramping state
    rampStartTime = 0;
    pwmA_current = 0;
    pwmA_target = 0;
    pwmB_current = 0;
    pwmB_target = 0;
    isRamping = false;
}

void Motor::setup()
{
    // Configure motor control pins as outputs
    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);
    pinMode(N_SLEEP, OUTPUT);

    // Configure ADC for current sensing
    analogReadResolution(12); // Set ADC to 12-bit resolution (0-4095)

    // Set all motor pins to LOW initially (brake mode)
    digitalWrite(PWM_A1, LOW);
    digitalWrite(PWM_A2, LOW);
    digitalWrite(PWM_B1, LOW);
    digitalWrite(PWM_B2, LOW);

    // Initialize DRV8243 driver
    initDRV8243();
}

void Motor::initDRV8243()
{
    // Note: DRV8243 nSLEEP pin handling would go here if available
    // For now, assuming driver is pre-configured or always enabled
    digitalWrite(N_SLEEP, HIGH);
    delay(2); // Wait 2ms for internal charge pumps to power up and stabilize

    // 4. Pulse nSLEEP LOW for 30 microseconds to clear latched faults
    digitalWrite(N_SLEEP, LOW);
    delayMicroseconds(30); // Must be strictly between 20us and 40us
    digitalWrite(N_SLEEP, HIGH);

    // Wait a moment for the fault logic to reset before driving
    delay(2);
}

void Motor::backward(int pwm)
{
    int pwmDeltaA = abs(pwm - pwmA_current);
    int pwmDeltaB = abs(pwm - pwmB_current);

    if (pwmDeltaA > DELTA_THRESHOLD || pwmDeltaB > DELTA_THRESHOLD)
    {
        // Large change: initiate ramp
        pwmA_target = pwm;
        pwmB_target = pwm;
        rampStartTime = millis();
        isRamping = true;

        // Update target direction pins immediately
        digitalWrite(PWM_A1, HIGH);
        digitalWrite(PWM_A2, LOW);
        digitalWrite(PWM_B1, HIGH);
        digitalWrite(PWM_B2, LOW);
    }
    else
    {
        // Small change: apply directly
        pwmA_current = pwm;
        pwmB_current = pwm;
        isRamping = false;

        analogWrite(PWM_A1, pwm); // Motor A forward
        digitalWrite(PWM_A2, LOW);

        analogWrite(PWM_B1, pwm); // Motor B forward
        digitalWrite(PWM_B2, LOW);
    }
}

void Motor::forward(int pwm)
{
    int pwmDeltaA = abs(pwm - pwmA_current);
    int pwmDeltaB = abs(pwm - pwmB_current);

    if (pwmDeltaA > DELTA_THRESHOLD || pwmDeltaB > DELTA_THRESHOLD)
    {
        // Large change: initiate ramp
        pwmA_target = pwm;
        pwmB_target = pwm;
        rampStartTime = millis();
        isRamping = true;

        // Update target direction pins immediately
        digitalWrite(PWM_A1, LOW);
        digitalWrite(PWM_A2, HIGH); // Will write actual PWM on next ramp update
        digitalWrite(PWM_B1, LOW);
        digitalWrite(PWM_B2, HIGH); // Will write actual PWM on next ramp update
    }
    else
    {
        // Small change: apply directly
        pwmA_current = pwm;
        pwmB_current = pwm;
        isRamping = false;

        digitalWrite(PWM_A1, LOW);
        analogWrite(PWM_A2, pwm); // Motor A reverse

        digitalWrite(PWM_B1, LOW);
        analogWrite(PWM_B2, pwm); // Motor B reverse
    }
}

void Motor::right(int pwm)
{
    int pwmDeltaA = abs(pwm - pwmA_current);
    int pwmDeltaB = abs(pwm - pwmB_current);

    if (pwmDeltaA > DELTA_THRESHOLD || pwmDeltaB > DELTA_THRESHOLD)
    {
        // Large change: initiate ramp
        pwmA_target = pwm;
        pwmB_target = pwm;
        rampStartTime = millis();
        isRamping = true;

        // Update target direction pins immediately
        digitalWrite(PWM_A1, HIGH); // Will write actual PWM on next ramp update
        digitalWrite(PWM_A2, LOW);

        digitalWrite(PWM_B1, LOW);
        digitalWrite(PWM_B2, HIGH); // Will write actual PWM on next ramp update
    }
    else
    {
        // Small change: apply directly
        pwmA_current = pwm;
        pwmB_current = pwm;
        isRamping = false;

        analogWrite(PWM_A1, pwm); // Motor A forward
        digitalWrite(PWM_A2, LOW);

        digitalWrite(PWM_B1, LOW);
        analogWrite(PWM_B2, pwm); // Motor B reverse
    }
}

void Motor::left(int pwm)
{
    int pwmDeltaA = abs(pwm - pwmA_current);
    int pwmDeltaB = abs(pwm - pwmB_current);

    if (pwmDeltaA > DELTA_THRESHOLD || pwmDeltaB > DELTA_THRESHOLD)
    {
        // Large change: initiate ramp
        pwmA_target = pwm;
        pwmB_target = pwm;
        rampStartTime = millis();
        isRamping = true;

        // Update target direction pins immediately
        digitalWrite(PWM_A1, LOW);
        digitalWrite(PWM_A2, HIGH); // Will write actual PWM on next ramp update

        digitalWrite(PWM_B1, HIGH); // Will write actual PWM on next ramp update
        digitalWrite(PWM_B2, LOW);
    }
    else
    {
        // Small change: apply directly
        pwmA_current = pwm;
        pwmB_current = pwm;
        isRamping = false;

        digitalWrite(PWM_A1, LOW);
        analogWrite(PWM_A2, pwm); // Motor A reverse

        analogWrite(PWM_B1, pwm); // Motor B forward
        digitalWrite(PWM_B2, LOW);
    }
}

void Motor::stop()
{
    digitalWrite(PWM_A1, LOW);
    digitalWrite(PWM_A2, LOW);
    digitalWrite(PWM_B1, LOW);
    digitalWrite(PWM_B2, LOW);

    // Stop ramping immediately
    isRamping = false;
    pwmA_current = 0;
    pwmB_current = 0;
    pwmA_target = 0;
    pwmB_target = 0;
}

void Motor::updateMotorRamp()
{
    // Only process if actively ramping
    if (!isRamping)
    {
        return;
    }

    // Calculate elapsed time since ramp start
    unsigned long elapsed = millis() - rampStartTime;

    // If ramp duration exceeded, finalize the ramp
    if (elapsed >= RAMP_DURATION_MS)
    {
        pwmA_current = pwmA_target;
        pwmB_current = pwmB_target;
        isRamping = false;
        applyRampedPWM(pwmA_current, pwmB_current);
        return;
    }

    // Calculate progress (0.0 to 1.0)
    float progress = (float)elapsed / RAMP_DURATION_MS;

    // Apply exponential easing: smoother than linear, natural acceleration feel
    // Formula: current = start + (target - start) * (1 - exp(-3 * progress))
    // This creates an ease-in curve that feels natural
    float easingFactor = 1.0f - expf(-3.0f * progress);

    // Interpolate motor A PWM
    pwmA_current = (int)(pwmA_target * easingFactor);

    // Interpolate motor B PWM
    pwmB_current = (int)(pwmB_target * easingFactor);

    // Apply the interpolated PWM values to the motors
    applyRampedPWM(pwmA_current, pwmB_current);
}

void Motor::applyRampedPWM(int currentA, int currentB)
{
    // Apply the ramped PWM values to the motor pins
    // Direction is already set by the motor control method (forward/backward/left/right)
    // We just need to update the PWM value on whichever pin is active for each motor

    // Motor A: determine which pin is active and update its PWM
    if (digitalRead(PWM_A1) == HIGH)
    {
        analogWrite(PWM_A1, currentA);
    }
    else
    {
        analogWrite(PWM_A2, currentA);
    }

    // Motor B: determine which pin is active and update its PWM
    if (digitalRead(PWM_B1) == HIGH)
    {
        analogWrite(PWM_B1, currentB);
    }
    else
    {
        analogWrite(PWM_B2, currentB);
    }
}

float Motor::readMotorCurrent()
{
    // 1. Read the raw ADC value (0 - 4095) from IPROPI pin
    int rawADC = analogRead(IPROPI_A_PIN);

    // 2. Convert raw ADC value to Voltage
    float voltage = (rawADC / 4095.0) * V_REF;

    // 3. Convert Voltage to Amps using IPROPI datasheet formula
    // Formula: I = (V / R) * A_factor
    float currentAmps = (voltage / R_IPROPI) * A_IPROPI;

    return currentAmps; // Return current in Amps
}

float Motor::readMotorBCurrent()
{
    // 1. Read the raw ADC value (0 - 4095) from IPROPI_B pin
    int rawADC = analogRead(IPROPI_B_PIN);

    // 2. Convert raw ADC value to Voltage
    float voltage = (rawADC / 4095.0) * V_REF;

    // 3. Convert Voltage to Amps using IPROPI datasheet formula
    // Formula: I = (V / R) * A_factor
    float currentAmps = (voltage / R_IPROPI) * A_IPROPI;

    return currentAmps; // Return current in Amps
}

float Motor::getFilteredMotorCurrent()
{
    // Get raw current reading for motor A
    float rawCurrent = readMotorCurrent();

    // On first read, initialize filtered value with raw value
    if (isFirstRead_A)
    {
        filteredCurrent_A = rawCurrent;
        isFirstRead_A = false;
        return filteredCurrent_A;
    }

    // Apply exponential moving average (alpha filter)
    // Formula: filtered = (ALPHA * rawValue) + ((1 - ALPHA) * previousFiltered)
    // With ALPHA=0.25: 25% new value, 75% previous (strong smoothing)
    filteredCurrent_A = (ALPHA_FILTER * rawCurrent) + ((1.0f - ALPHA_FILTER) * filteredCurrent_A);

    return filteredCurrent_A;
}

float Motor::getFilteredMotorBCurrent()
{
    // Get raw current reading for motor B
    float rawCurrent = readMotorBCurrent();

    // On first read, initialize filtered value with raw value
    if (isFirstRead_B)
    {
        filteredCurrent_B = rawCurrent;
        isFirstRead_B = false;
        return filteredCurrent_B;
    }

    // Apply exponential moving average (alpha filter)
    // Formula: filtered = (ALPHA * rawValue) + ((1 - ALPHA) * previousFiltered)
    // With ALPHA=0.25: 25% new value, 75% previous (strong smoothing)
    filteredCurrent_B = (ALPHA_FILTER * rawCurrent) + ((1.0f - ALPHA_FILTER) * filteredCurrent_B);

    return filteredCurrent_B;
}

void Motor::updatePeaks()
{
    // Update peak for motor A
    float currentA = getFilteredMotorCurrent();
    if (currentA > peakCurrent_A)
    {
        peakCurrent_A = currentA;
    }

    // Update peak for motor B
    float currentB = getFilteredMotorBCurrent();
    if (currentB > peakCurrent_B)
    {
        peakCurrent_B = currentB;
    }
}

float Motor::getPeakMotorACurrent()
{
    return peakCurrent_A;
}

float Motor::getPeakMotorBCurrent()
{
    return peakCurrent_B;
}

float Motor::getTotalPeakCurrent()
{
    return peakCurrent_A + peakCurrent_B;
}

void Motor::resetPeaks()
{
    peakCurrent_A = 0.0f;
    peakCurrent_B = 0.0f;
}

int Motor::getPWM_A_Current() const
{
    return pwmA_current;
}

int Motor::getPWM_B_Current() const
{
    return pwmB_current;
}
