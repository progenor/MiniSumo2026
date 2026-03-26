#include "motors.h"
#include "pins.h"

Motor::Motor()
{
    // Constructor - pins will be initialized in setup()
}

void Motor::setup()
{
    // Configure motor control pins as outputs
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
    analogWrite(PWM_A1, pwm); // Motor A forward
    digitalWrite(PWM_A2, LOW);

    analogWrite(PWM_B1, pwm); // Motor B forward
    digitalWrite(PWM_B2, LOW);
}

void Motor::backward(int pwm)
{
    digitalWrite(PWM_A1, LOW);
    analogWrite(PWM_A2, pwm); // Motor A reverse

    digitalWrite(PWM_B1, LOW);
    analogWrite(PWM_B2, pwm); // Motor B reverse
}

void Motor::left(int pwm)
{
    analogWrite(PWM_A1, pwm); // Motor A forward
    digitalWrite(PWM_A2, LOW);

    digitalWrite(PWM_B1, LOW);
    analogWrite(PWM_B2, pwm); // Motor B reverse
}

void Motor::right(int pwm)
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
