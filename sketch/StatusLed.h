#ifndef STATUS_LED_H
#define STATUS_LED_H

/**
 * @file StatusLed.h
 * @brief Simple LED on a digital pin (wired through a 220 ohm resistor).
 * @author ResQ IoT development team
 */

#include <Arduino.h>

/**
 * @class StatusLed
 * @brief Turns one LED on and off and remembers its state.
 */
class StatusLed {
public:
    /**
     * @brief Builds the LED.
     * @param ledPin Pin wired to the resistor that goes to the LED anode.
     */
    explicit StatusLed(int ledPin) : ledPin(ledPin), isOn(false) {}

    /** @brief Configures the pin and starts with the LED off. Call it once from setup(). */
    void begin() {
        pinMode(ledPin, OUTPUT);
        setOn(false);
    }

    /**
     * @brief Turns the LED on or off.
     * @param shouldBeOn true to turn it on.
     */
    void setOn(bool shouldBeOn) {
        isOn = shouldBeOn;
        digitalWrite(ledPin, isOn ? HIGH : LOW);
    }

    /**
     * @brief Current state.
     * @return true while the LED is on.
     */
    bool getIsOn() const { return isOn; }

private:
    int ledPin;
    bool isOn;
};

#endif // STATUS_LED_H
