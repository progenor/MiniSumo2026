#include <Arduino.h>
#include "motors.h"
#include "pins.h"
#define IRCount 3

int sensorValues[IRCount]; // Filtered sensor values
int rawValues[IRCount];    // Raw unfiltered sensor values
int debounceCounter[IRCount];

void initDRV8243()
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
    Serial.println("fwd" + String(pwm));
    digitalWrite(PWM_A1, LOW);
    analogWrite(PWM_A2, pwm); // Motor A reverse

    digitalWrite(PWM_B1, pwm);
    analogWrite(PWM_B2, LOW); // Motor B reverse
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

    // Set all motor pins to LOW initially (brake mode)
    digitalWrite(PWM_A1, LOW);
    digitalWrite(PWM_A2, LOW);
    digitalWrite(PWM_B1, LOW);
    digitalWrite(PWM_B2, LOW);

    initDRV8243();
}

void loop()
{
    forward(128);
    delay(500);
}