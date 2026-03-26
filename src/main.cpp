
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Display Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1

// --- Pin Definitions (From your Pico Schematic) ---
#define PWM_A1 9   // GP9 controls Driver A IN1
#define PWM_A2 8   // GP8 controls Driver A IN2
#define PWM_B1 20  // GP20 controls Driver B IN1
#define PWM_B2 21  // GP21 controls Driver B IN2
#define N_SLEEP 22 // GP22 controls both drivers' nSLEEP pins
#define BUZZER 3

// --- Sensor Configuration ---
#define QTRSensorCount 8

// Display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayInit()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("SSD1306 initialization failed!");
        while (true)
            ;
    }
}

void displayClear()
{
    display.clearDisplay();
    display.display();
}

void displayPrint(const char *text, double value = -1)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(text);
    if (value != -1)
    {
        display.println(value);
    }
    display.display();
    delay(200);
}

void display_IR(uint16_t *irValues)
{
    display.clearDisplay();
    uint8_t bar_width = SCREEN_WIDTH / QTRSensorCount;
    if (bar_width < 2)
        bar_width = 2;

    for (uint8_t i = 0; i < QTRSensorCount; i++)
    {
        uint16_t ir = irValues[i];
        if (ir > 2500)
            ir = 2500;
        uint8_t bar_height = (ir * SCREEN_HEIGHT) / 2500;
        uint8_t x = i * bar_width;
        uint8_t y = SCREEN_HEIGHT - bar_height;
        display.fillRect(x, y, bar_width - 1, bar_height, SSD1306_WHITE);
    }
    display.display();
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    // Initialize I2C
    Wire.begin(); // SDA on GPIO4, SCL on GPIO5

    // Initialize OLED display
    displayInit();
    displayPrint("SpeedyBee!");
    delay(1000);

    // 1. Configure pins as outputs
    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);
    pinMode(N_SLEEP, OUTPUT);

    // 2. Start with all PWM pins LOW (Brake mode)
    digitalWrite(PWM_A1, LOW);
    digitalWrite(PWM_A2, LOW);
    digitalWrite(PWM_B1, LOW);
    digitalWrite(PWM_B2, LOW);

    // 3. Wake up the DRV8243-Q1
    digitalWrite(N_SLEEP, HIGH);
    delay(2); // Wait 2ms for internal charge pumps to power up and stabilize

    // 4. Pulse nSLEEP LOW for 30 microseconds to clear latched faults
    digitalWrite(N_SLEEP, LOW);
    delayMicroseconds(30); // Must be strictly between 20us and 40us
    digitalWrite(N_SLEEP, HIGH);

    // Wait a moment for the fault logic to reset before driving
    delay(2);

    analogWrite(BUZZER, 255);
    delay(500);
    analogWrite(BUZZER, 0);
}

void loop()
{
    // --- DRIVE MOTORS FORWARD ---
    // To drive forward: IN1 = PWM, IN2 = LOW
    // Pico analogWrite uses 8-bit resolution by default (0 - 255)

    analogWrite(PWM_A1, 127); // Motor A at ~50% duty cycle
    digitalWrite(PWM_A2, LOW);

    analogWrite(PWM_B1, 127); // Motor B at ~50% duty cycle
    digitalWrite(PWM_B2, LOW);

    Serial.println("Motors Driving Forward...");
    delay(3000); // Run for 3 seconds

    // --- BRAKE MOTORS ---
    // To brake: IN1 = LOW, IN2 = LOW
    analogWrite(PWM_A1, 0); // Turn off PWM
    digitalWrite(PWM_A2, LOW);

    analogWrite(PWM_B1, 0); // Turn off PWM
    digitalWrite(PWM_B2, LOW);

    Serial.println("Motors Braking...");
    delay(2000); // Stop for 2 seconds
}
