#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "defines.h"

class Display
{
public:
    Display();

    // Initialize OLED display
    bool setup();

    // Display text with optional numeric value
    void print(const char *text, double value = -1);

    // Clear display
    void clear();

    // Display IR sensor bars (0-1 digital values)
    void displayIR(int *irValues, int sensorCount);

    // Throttled display update (respects DISPLAY_REFRESH_MS)
    bool shouldUpdate();

private:
    Adafruit_SSD1306 oled;
    unsigned long lastUpdateTime;

    // Helper to draw bar graph
    void drawBarGraph(uint8_t *values, uint8_t count, const char *label);
};

#endif // DISPLAY_H
