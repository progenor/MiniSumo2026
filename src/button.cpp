#include "button.h"
#include "pins.h"

ButtonManager::ButtonManager()
    : currentState(BUTTON_STATE_IDLE),
      previousState(BUTTON_STATE_IDLE),
      detectedGesture(GESTURE_NONE),
      lastDebounceTime(0),
      pressStartTime(0),
      lastPressTime(0),
      pressCount(0)
{
}

void ButtonManager::setup()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    currentState = BUTTON_STATE_IDLE;
    previousState = BUTTON_STATE_IDLE;
}

void ButtonManager::update()
{
    updateStateMachine();
    detectGesture();
}

ButtonGesture ButtonManager::getGesture()
{
    ButtonGesture gesture = detectedGesture;
    detectedGesture = GESTURE_NONE; // Auto-reset after reading
    return gesture;
}

ButtonState ButtonManager::getState() const
{
    return currentState;
}

bool ButtonManager::isPressed() const
{
    return currentState == BUTTON_STATE_PRESSED || currentState == BUTTON_STATE_HELD;
}

bool ButtonManager::readRawButton()
{
    // Button is active low (INPUT_PULLUP)
    return digitalRead(BUTTON_PIN) == LOW;
}

void ButtonManager::updateStateMachine()
{
    unsigned long currentTime = millis();
    bool buttonPressed = readRawButton();

    // State machine with debouncing
    switch (currentState)
    {
    case BUTTON_STATE_IDLE:
        if (buttonPressed && (currentTime - lastDebounceTime) > BUTTON_DEBOUNCE_MS)
        {
            currentState = BUTTON_STATE_PRESSED;
            pressStartTime = currentTime;
            lastDebounceTime = currentTime;
        }
        break;

    case BUTTON_STATE_PRESSED:
        if (!buttonPressed && (currentTime - lastDebounceTime) > BUTTON_DEBOUNCE_MS)
        {
            // Button released (short press)
            currentState = BUTTON_STATE_RELEASED;
            lastDebounceTime = currentTime;
            lastPressTime = currentTime;
            pressCount++;
        }
        else if (buttonPressed && (currentTime - pressStartTime) >= BUTTON_LONG_PRESS_MS)
        {
            // Button held for long press threshold
            currentState = BUTTON_STATE_HELD;
        }
        break;

    case BUTTON_STATE_HELD:
        if (!buttonPressed && (currentTime - lastDebounceTime) > BUTTON_DEBOUNCE_MS)
        {
            // Long press button released
            currentState = BUTTON_STATE_RELEASED;
            lastDebounceTime = currentTime;
        }
        break;

    case BUTTON_STATE_RELEASED:
        // Check for double-press window or timeout to finalize gesture
        if ((currentTime - lastPressTime) > BUTTON_DOUBLE_PRESS_WINDOW_MS)
        {
            // Double-press window closed, finalize gesture
            if (pressCount == 1)
            {
                // Was a long press
                if ((currentTime - pressStartTime) >= BUTTON_LONG_PRESS_MS)
                {
                    detectedGesture = GESTURE_LONG_PRESS;
                }
                else
                {
                    // Single short press
                    detectedGesture = GESTURE_SINGLE_PRESS;
                }
            }
            else if (pressCount == 2)
            {
                // Double press
                detectedGesture = GESTURE_DOUBLE_PRESS;
            }
            pressCount = 0;
            currentState = BUTTON_STATE_IDLE;
        }
        else if (buttonPressed && (currentTime - lastDebounceTime) > BUTTON_DEBOUNCE_MS)
        {
            // Second press within double-press window
            currentState = BUTTON_STATE_PRESSED;
            pressStartTime = currentTime;
            lastDebounceTime = currentTime;
        }
        break;
    }

    previousState = currentState;
}

void ButtonManager::detectGesture()
{
    unsigned long currentTime = millis();

    // Timeout long-press detection if button held beyond threshold
    if (currentState == BUTTON_STATE_HELD && detectedGesture == GESTURE_NONE)
    {
        detectedGesture = GESTURE_LONG_PRESS;
    }
}
