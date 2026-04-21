#include "motors.h"
#include "pins.h"
#include "hardware/pwm.h"

// Current sensing constants (IPROPI configuration)
const float R_IPROPI = 1000.0; // Resistor value in Ohms (1kΩ)
const float A_IPROPI = 3075.0; // Datasheet scaling factor
const float V_REF = 3.3;       // Pico reference voltage

// Alpha filter coefficient for current smoothing
const float Motor::ALPHA_FILTER = 0.97; // 3% new value, 97% previous (very strong smoothing/low-pass filter)

// Peak reset threshold
const float Motor::PEAK_RESET_THRESHOLD = 0.5f; // 0.5 Amps

Motor::Motor()
{
    // Constructor - pins will be initialized in setup()
    filteredCurrent_A = 0.0f;
    filteredCurrent_B = 0.0f;
    isFirstRead_A = true;
    isFirstRead_B = true;
    peakCurrent_A = 0.0f;
    peakCurrent_B = 0.0f;
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
    // Configure PWM frequency to 20 kHz for optimal DRV8243 high-current mode
    // DRV8243 requires high frequency PWM (20 kHz typical) for stable current control
    // at mid-range PWM values. Default 1 kHz causes faults at PWM < 200.
    uint slice_A = pwm_gpio_to_slice_num(PWM_A1); // GPIO 9 - Motor A
    uint slice_B = pwm_gpio_to_slice_num(PWM_B1); // GPIO 20 - Motor B

    // Set PWM frequency to 20 kHz by calculating wrap value
    // Formula: freq = clock_freq / (top+1) where clock_freq = 125 MHz, divisor = 1
    // For 20 kHz: top = (125MHz / 20kHz) - 1 = 6249
    uint16_t wrap = 6249; // 125MHz / (6249+1) = ~20kHz
    pwm_set_wrap(slice_A, wrap);
    pwm_set_wrap(slice_B, wrap);
    pwm_set_enabled(slice_A, true);
    pwm_set_enabled(slice_B, true);

    // Set all motor pins to LOW initially (brake mode)
    digitalWrite(PWM_A1, LOW);
    digitalWrite(PWM_A2, LOW);
    digitalWrite(PWM_B1, LOW);
    digitalWrite(PWM_B2, LOW);

    // Load previously saved peak values from ROM
    loadPeaksFromROM();

    // Check and reset any peaks exceeding threshold
    checkAndResetHighPeaks();

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

void Motor::forward(int pwm)
{
    Serial.println("back" + String(pwm));
    analogWrite(PWM_A1, pwm); // Motor A forward
    digitalWrite(PWM_A2, LOW);

    analogWrite(PWM_B1, LOW); // Motor B forward
    digitalWrite(PWM_B2, pwm);
}

void Motor::backward(int pwm)
{
    Serial.println("fwd" + String(pwm));
    digitalWrite(PWM_A1, LOW);
    analogWrite(PWM_A2, pwm); // Motor A reverse

    digitalWrite(PWM_B1, pwm);
    analogWrite(PWM_B2, LOW); // Motor B reverse
}

void Motor::right(int pwm)
{
    Serial.println("right" + String(pwm));
    analogWrite(PWM_A1, LOW); // Motor A forward
    digitalWrite(PWM_A2, pwm);

    digitalWrite(PWM_B1, LOW);
    analogWrite(PWM_B2, pwm); // Motor B reverse
}

void Motor::left(int pwm)
{
    Serial.println("left" + String(pwm));
    digitalWrite(PWM_A1, pwm);
    analogWrite(PWM_A2, LOW); // Motor A reverse

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

    // Check and reset any peaks exceeding threshold
    checkAndResetHighPeaks();
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

void Motor::savePeaksToROM()
{
    // Ensure EEPROM is initialized
    EEPROM.begin(EEPROM_SIZE);

    // Write peak A current (float = 4 bytes)
    EEPROM.put(EEPROM_ADDR_PEAK_A, peakCurrent_A);

    // Write peak B current (float = 4 bytes)
    EEPROM.put(EEPROM_ADDR_PEAK_B, peakCurrent_B);

    // Commit changes to flash
    EEPROM.commit();

    Serial.print("Peaks saved to ROM - Motor A: ");
    Serial.print(peakCurrent_A);
    Serial.print(" A, Motor B: ");
    Serial.print(peakCurrent_B);
    Serial.println(" A");
}

void Motor::loadPeaksFromROM()
{
    // Ensure EEPROM is initialized
    EEPROM.begin(EEPROM_SIZE);

    // Read peak A current from ROM
    EEPROM.get(EEPROM_ADDR_PEAK_A, peakCurrent_A);

    // Read peak B current from ROM
    EEPROM.get(EEPROM_ADDR_PEAK_B, peakCurrent_B);

    // Validate loaded values - uninitialized EEPROM may contain garbage (0xFF)
    // that becomes NaN or infinite when read as float
    if (isnan(peakCurrent_A) || isinf(peakCurrent_A) || peakCurrent_A < 0)
    {
        Serial.println("Motor A peak value invalid (NaN/Inf/Negative) - resetting to 0");
        peakCurrent_A = 0.0f;
    }

    if (isnan(peakCurrent_B) || isinf(peakCurrent_B) || peakCurrent_B < 0)
    {
        Serial.println("Motor B peak value invalid (NaN/Inf/Negative) - resetting to 0");
        peakCurrent_B = 0.0f;
    }

    Serial.print("Peaks loaded from ROM - Motor A: ");
    Serial.print(peakCurrent_A);
    Serial.print(" A, Motor B: ");
    Serial.print(peakCurrent_B);
    Serial.println(" A");
}

void Motor::clearPeakROM()
{
    // Ensure EEPROM is initialized
    EEPROM.begin(EEPROM_SIZE);

    // Write zeros to both peak values
    float zero = 0.0f;
    EEPROM.put(EEPROM_ADDR_PEAK_A, zero);
    EEPROM.put(EEPROM_ADDR_PEAK_B, zero);

    // Commit changes to flash
    EEPROM.commit();

    Serial.println("Peak values cleared from ROM");
}

void Motor::checkAndResetHighPeaks()
{
    // Check Motor A peak
    if (peakCurrent_A > PEAK_RESET_THRESHOLD)
    {
        Serial.print("Motor A peak (");
        Serial.print(peakCurrent_A);
        Serial.println("A) exceeds 0.5A threshold - RESET");
        peakCurrent_A = 0.0f;
    }

    // Check Motor B peak
    if (peakCurrent_B > PEAK_RESET_THRESHOLD)
    {
        Serial.print("Motor B peak (");
        Serial.print(peakCurrent_B);
        Serial.println("A) exceeds 0.5A threshold - RESET");
        peakCurrent_B = 0.0f;
    }
}
