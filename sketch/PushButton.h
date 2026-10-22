#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H

/**
 * @file PushButton.h
 * @brief Push button wired to GND with the internal pull-up (for example the BOOT button, GPIO 0).
 *
 * Released = HIGH, pressed = LOW. The class reports only the moment it is pressed
 * (the HIGH -> LOW edge), so holding it down counts as one press.
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>

/**
 * @class PushButton
 * @brief Detects single presses of a button, ignoring contact bounce.
 */
class PushButton {
public:
    static constexpr unsigned long DEBOUNCE_TIME_MS = 200;

    /**
     * @brief Builds the button.
     * @param buttonPin Pin wired to the button (the other leg goes to GND).
     */
    explicit PushButton(int buttonPin)
        : buttonPin(buttonPin), lastState(HIGH), lastPressMs(0) {}

    /** @brief Configures the pin with pull-up. Call it once from setup(). */
    void begin() {
        pinMode(buttonPin, INPUT_PULLUP);
        lastState = digitalRead(buttonPin);
    }

    /**
     * @brief Checks the button. Call it often (on every loop).
     * @return true only once per press.
     */
    bool wasPressed() {
        int currentState = digitalRead(buttonPin);
        bool isNewPress = lastState == HIGH && currentState == LOW
                          && millis() - lastPressMs >= DEBOUNCE_TIME_MS;
        lastState = currentState;
        if (isNewPress) lastPressMs = millis();
        return isNewPress;
    }

private:
    int buttonPin;
    int lastState;
    unsigned long lastPressMs;
};

#endif // PUSH_BUTTON_H
