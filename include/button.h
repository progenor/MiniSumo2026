#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

// Button state enumeration
enum ButtonState
{
    BUTTON_STATE_IDLE,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_HELD,
    BUTTON_STATE_RELEASED
};

// Button gesture enumeration
enum ButtonGesture
{
    GESTURE_NONE,
    GESTURE_SINGLE_PRESS,
    GESTURE_DOUBLE_PRESS,
    GESTURE_LONG_PRESS
};

// Button timing constants
#define BUTTON_DEBOUNCE_MS 20
#define BUTTON_LONG_PRESS_MS 1000
#define BUTTON_DOUBLE_PRESS_WINDOW_MS 400

/**
 * ButtonManager
 * Handles button input with debouncing and gesture detection
 * - Single press: Quick press and release
 * - Double press: Two presses within 400ms window
 * - Long press: Press held for >1 second
 */
class ButtonManager
{
public:
    ButtonManager();

    /**
     * Initialize button pin
     */
    void setup();

    /**
     * Update button state (call this in main loop, ~200Hz)
     * Handles debouncing and gesture timing
     */
    void update();

    /**
     * Get the current gesture
     * Auto-resets to GESTURE_NONE after being read once
     * @return ButtonGesture - detected gesture or GESTURE_NONE
     */
    ButtonGesture getGesture();

    /**
     * Get current button state (raw, for debugging)
     * @return ButtonState - current physical button state
     */
    ButtonState getState() const;

    /**
     * Check if button is physically pressed
     * @return bool - true if button is currently pressed
     */
    bool isPressed() const;

private:
    ButtonState currentState;
    ButtonState previousState;
    ButtonGesture detectedGesture;

    unsigned long lastDebounceTime;
    unsigned long pressStartTime;
    unsigned long lastPressTime;
    int pressCount;

    /**
     * Read raw button value (active low, INPUT_PULLUP)
     * @return bool - true if button is physically pressed
     */
    bool readRawButton();

    /**
     * Internal state machine for handling button transitions
     */
    void updateStateMachine();

    /**
     * Detect and record gesture based on press timing
     */
    void detectGesture();
};

#endif // BUTTON_H
