#ifndef ALARM_BUZZER_H
#define ALARM_BUZZER_H

/**
 * @file AlarmBuzzer.h
 * @brief Active buzzer (the one in the MVP list): it beeps by itself when its pin is HIGH.
 *
 * An active buzzer has its own oscillator inside, so it only needs ON/OFF.
 * (A passive piezo would need tone() with a frequency instead.)
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>

/**
 * @class AlarmBuzzer
 * @brief Turns the audible alarm on and off.
 */
class AlarmBuzzer {
public:
    /**
     * @brief Builds the buzzer.
     * @param signalPin Pin wired to the buzzer "+" leg.
     */
    explicit AlarmBuzzer(int signalPin) : signalPin(signalPin), isSounding(false) {}

    /** @brief Configures the pin and starts silent. Call it once from setup(). */
    void begin() {
        pinMode(signalPin, OUTPUT);
        setSounding(false);
    }

    /**
     * @brief Starts or stops the alarm.
     * @param shouldSound true to make it beep.
     */
    void setSounding(bool shouldSound) {
        isSounding = shouldSound;
        digitalWrite(signalPin, isSounding ? HIGH : LOW);
    }

    /**
     * @brief Current state.
     * @return true while the buzzer beeps.
     */
    bool getIsSounding() const { return isSounding; }

private:
    int signalPin;
    bool isSounding;
};

#endif // ALARM_BUZZER_H
