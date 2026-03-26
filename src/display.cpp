#include "display.h"

Display::Display()
    : oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET),
      lastUpdateTime(0)
{
}

bool Display::setup()
{
    if (!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("SSD1306 initialization failed!");
        return false;
    }
    oled.clearDisplay();
    oled.setTextColor(1);
    oled.display();
    return true;
}

void Display::print(const char *text, double value)
{
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0, 0);
    oled.print(text);
    if (value != -1)
    {
        oled.println(value);
    }
    oled.display();
}

void Display::clear()
{
    oled.clearDisplay();
    oled.display();
}

void Display::displayIR(int *irValues, int sensorCount)
{
    if (!shouldUpdate())
        return; // Throttle updates

    oled.clearDisplay();
    uint8_t bar_width = SCREEN_WIDTH / sensorCount;
    if (bar_width < 2)
        bar_width = 2;

    for (uint8_t i = 0; i < sensorCount; i++)
    {
        int ir = irValues[i];

        // If sensor reads 1 (HIGH), draw full height; else 0
        uint8_t bar_height = (ir == 1) ? SCREEN_HEIGHT : 0;
        uint8_t x = i * bar_width;
        uint8_t y = SCREEN_HEIGHT - bar_height;

        if (bar_height > 0)
        {
            oled.fillRect(x, y, bar_width - 1, bar_height, SSD1306_WHITE);
        }
    }
    oled.display();
}

bool Display::shouldUpdate()
{
    unsigned long now = millis();
    if (now - lastUpdateTime >= DISPLAY_REFRESH_MS)
    {
        lastUpdateTime = now;
        return true;
    }
    return false;
}

void Display::drawLoadingScreen(const char *status)
{
    oled.clearDisplay();

    // Draw custom bitmap logo
    oled.drawBitmap(42, 2, image_Pasted_image_bits, 45, 50, 1);
    // Layer 2
    oled.setTextColor(1);
    oled.setTextWrap(false);
    oled.setCursor(35, 53);
    oled.print("Loading...");

    oled.display();
}
