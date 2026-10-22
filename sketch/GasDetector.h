#ifndef GAS_DETECTOR_H
#define GAS_DETECTOR_H

/**
 * @file GasDetector.h
 * @brief MQ-2 gas/smoke sensor read through its analog output (AO).
 *
 * The ESP32 turns the AO voltage into a raw ADC value from 0 to 4095. The value is NOT
 * calibrated in ppm and the MQ-2 cannot tell gas from smoke: it only tells "how much".
 * The real MQ-2 needs a warm-up time before its readings are stable.
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>

/**
 * @class GasDetector
 * @brief Reads the MQ-2 and remembers the latest raw value.
 */
class GasDetector {
public:
    static constexpr int MIN_RAW_LEVEL = 0;       ///< lowest ADC value (12 bits)
    static constexpr int MAX_RAW_LEVEL = 4095;    ///< highest ADC value (12 bits)

    /**
     * @brief Builds the detector.
     * @param analogPin ESP32 ADC1 pin wired to the MQ-2 AO output (through the divider).
     * @param warmUpTimeMs Time after power-on during which the readings are not trusted.
     */
    GasDetector(int analogPin, unsigned long warmUpTimeMs)
        : analogPin(analogPin), warmUpTimeMs(warmUpTimeMs), rawLevel(MIN_RAW_LEVEL) {}

    /** @brief Configures the pin. Call it once from setup(). */
    void begin() {
        pinMode(analogPin, INPUT);
    }

    /**
     * @brief Takes a new reading.
     * @return Raw ADC value between MIN_RAW_LEVEL and MAX_RAW_LEVEL.
     */
    int read() {
        rawLevel = constrain(analogRead(analogPin), MIN_RAW_LEVEL, MAX_RAW_LEVEL);
        return rawLevel;
    }

    /**
     * @brief Latest raw reading (does not read the sensor again).
     * @return Raw ADC value.
     */
    int getRawLevel() const { return rawLevel; }

    /**
     * @brief Tells whether the heater of the sensor had enough time to stabilize.
     * @return true after the warm-up time.
     */
    bool isWarmedUp() const { return millis() >= warmUpTimeMs; }

    /**
     * @brief Seconds left until the sensor is ready.
     * @return Remaining warm-up time in seconds (0 when ready).
     */
    unsigned long getRemainingWarmUpSeconds() const {
        return isWarmedUp() ? 0 : (warmUpTimeMs - millis()) / MILLISECONDS_PER_SECOND;
    }

private:
    static constexpr unsigned long MILLISECONDS_PER_SECOND = 1000;

    int analogPin;
    unsigned long warmUpTimeMs;
    int rawLevel;
};

#endif // GAS_DETECTOR_H
