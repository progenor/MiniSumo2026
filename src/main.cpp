#include <Arduino.h>
#include "hardware/pwm.h"
#define IRCount 3

// Motor Control Pins
#define PWM_A1 9   // GP9 controls Driver A IN1
#define PWM_A2 8   // GP8 controls Driver A IN2
#define PWM_B1 20  // GP20 controls Driver B IN1
#define PWM_B2 21  // GP21 controls Driver B IN2
#define N_SLEEP 22 // GP22 controls both drivers' nSLEEP pins

// Buzzer Pin
#define BUZZER 3

// IR Sensor Pins
#define SENSOR_LEFT 12
#define SENSOR_CENTER 13
#define SENSOR_RIGHT 14

// Button Pin
#define BUTTON_PIN 15

const uint8_t IRPins[] = {SENSOR_LEFT, SENSOR_CENTER, SENSOR_RIGHT};

void setupPins();

int sensorValues[IRCount]; // Filtered sensor values
int rawValues[IRCount];    // Raw unfiltered sensor values
int debounceCounter[IRCount];

void initDRV8243()
{
    // Note: DRV8243 nSLEEP pin handling would go here if available
    // For now, assuming driver is pre-configured or always enabled
    digitalWrite(N_SLEEP, HIGH);
    delay(2); // Wait 2ms for internal charge pumps to power up and stabilize

    // // 4. Pulse nSLEEP LOW for 30 microseconds to clear latched faults
    // digitalWrite(N_SLEEP, LOW);
    // delayMicroseconds(30); // Must be strictly between 20us and 40us
    // digitalWrite(N_SLEEP, HIGH);

    // Wait a moment for the fault logic to reset before driving
    delay(2);
}

void backward(int pwm)
{
    Serial.println("back" + String(pwm));
    analogWrite(PWM_A1, pwm); // Motor A forward
    digitalWrite(PWM_A2, LOW);

    analogWrite(PWM_B1, LOW); // Motor B forward
    digitalWrite(PWM_B2, pwm);
}

void forward(int pwm)
{
    analogWrite(PWM_A1, pwm); // Motor A forward
    digitalWrite(PWM_A2, LOW);

    analogWrite(PWM_B1, LOW); // Motor B forward
    digitalWrite(PWM_B2, pwm);
}

void right(int pwm)
{
    Serial.println("right" + String(pwm));
    analogWrite(PWM_A1, LOW); // Motor A forward
    digitalWrite(PWM_A2, pwm);

    digitalWrite(PWM_B1, LOW);
    analogWrite(PWM_B2, pwm); // Motor B reverse
}

void left(int pwm)
{
    Serial.println("left" + String(pwm));
    digitalWrite(PWM_A1, pwm);
    analogWrite(PWM_A2, LOW); // Motor A reverse

    analogWrite(PWM_B1, pwm); // Motor B forward
    digitalWrite(PWM_B2, LOW);
}

void stop()
{
    digitalWrite(PWM_A1, LOW);
    digitalWrite(PWM_A2, LOW);
    digitalWrite(PWM_B1, LOW);
    digitalWrite(PWM_B2, LOW);
}

void read()
{
    // Read raw values from IR sensors with debounce filtering
    // Debounce Filter: Requires DEBOUNCE_THRESHOLD consecutive stable readings
    // before accepting a state change. This filters out transient spikes from
    // ground bounce during fast turns and other noise.

    for (int i = 0; i < IRCount; i++)
    {
        int rawRead = digitalRead(IRPins[i]);
        sensorValues[i] = rawRead;
    }
}

void setup()
{
    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);
    pinMode(N_SLEEP, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

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

    initDRV8243();
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH); // Ensure buzzer is off
    delay(1000);                // Short delay to ensure buzzer state is stable
    digitalWrite(BUZZER, LOW);  // Turn on buzzer briefly for startup sound
    delay(200);
}
int pwm = 255;
void loop()
{
    if (digitalRead(BUTTON_PIN) == LOW) // Button pressed (active LOW)
    {
        delay(2);
        // // 4. Pulse nSLEEP LOW for 30 microseconds to clear latched faults
        digitalWrite(N_SLEEP, LOW);
        delayMicroseconds(30); // Must be strictly between 20us and 40us
        digitalWrite(N_SLEEP, HIGH);
        delay(2);
    }
    if (pwm <= 0)
    {
        pwm = 255;
    }
    forward(pwm--);

    delay(5);
}