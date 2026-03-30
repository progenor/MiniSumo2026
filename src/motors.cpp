#include "motors.h"
#include "pins.h"

// Current sensing constants (IPROPI configuration)
const float R_IPROPI = 1000.0; // Resistor value in Ohms (1kΩ)
const float A_IPROPI = 3075.0; // Datasheet scaling factor
const float V_REF = 3.3;       // Pico reference voltage

// Alpha filter coefficient for current smoothing
const float Motor::ALPHA_FILTER = 0.97; // 3% new value, 97% previous (very strong smoothing/low-pass filter)

Motor::Motor()
{
    // Constructor - pins will be initialized in setup()
    filteredCurrent_A = 0.0f;
    filteredCurrent_B = 0.0f;
    isFirstRead_A = true;
    isFirstRead_B = true;
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
    analogWrite(PWM_A1, pwm); // Motor A forward
    digitalWrite(PWM_A2, LOW);

    analogWrite(PWM_B1, pwm); // Motor B forward
    digitalWrite(PWM_B2, LOW);
}

void Motor::forward(int pwm)
{
    digitalWrite(PWM_A1, LOW);
    analogWrite(PWM_A2, pwm); // Motor A reverse

    digitalWrite(PWM_B1, LOW);
    analogWrite(PWM_B2, pwm); // Motor B reverse
}

void Motor::right(int pwm)
{
    analogWrite(PWM_A1, pwm); // Motor A forward
    digitalWrite(PWM_A2, LOW);

    digitalWrite(PWM_B1, LOW);
    analogWrite(PWM_B2, pwm); // Motor B reverse
}

void Motor::left(int pwm)
{
    digitalWrite(PWM_A1, LOW);
    analogWrite(PWM_A2, pwm); // Motor A reverse

    analogWrite(PWM_B1, pwm); // Motor B forward
    digitalWrite(PWM_B2, LOW);
}

void Motor::stop()
{
    digitalWrite(PWM_A1, LOW);
    digitalWrite(PWM_A2, LOW);
    digitalWrite(PWM_B1, LOW);
    digitalWrite(PWM_B2, LOW);
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
