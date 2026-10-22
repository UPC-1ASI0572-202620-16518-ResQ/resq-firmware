#ifndef MOTION_DETECTOR_H
#define MOTION_DETECTOR_H

/**
 * @file MotionDetector.h
 * @brief HC-SR501 PIR motion sensor: its OUT pin is HIGH while it sees movement.
 *
 * Wiring: VCC to VIN (5 V), GND to GND, OUT to GPIO 33. OUT gives 3.3 V, so it
 * needs no voltage divider.
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>

/**
 * @class MotionDetector
 * @brief Tells whether there is a person moving in the room.
 */
class MotionDetector {
public:
    /**
     * @brief Builds the detector.
     * @param signalPin Pin wired to the PIR OUT leg.
     */
    explicit MotionDetector(int signalPin) : signalPin(signalPin), isMotionDetected(false) {}

    /** @brief Configures the pin. Call it once from setup(). */
    void begin() { pinMode(signalPin, INPUT); }

    /**
     * @brief Takes a new reading.
     * @return true while the PIR sees movement.
     */
    bool read() {
        isMotionDetected = digitalRead(signalPin) == HIGH;
        return isMotionDetected;
    }

    /**
     * @brief Last reading (does not read the sensor again).
     * @return true if there was movement.
     */
    bool getIsMotionDetected() const { return isMotionDetected; }

private:
    int signalPin;
    bool isMotionDetected;
};

#endif // MOTION_DETECTOR_H
