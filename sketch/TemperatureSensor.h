#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

/**
 * @file TemperatureSensor.h
 * @brief DHT22 temperature sensor (the one in the team's Cirkit design).
 *
 * Wiring: VCC to 3V3, GND to GND, DATA to GPIO 4 (same pin as the lab guide).
 * The DHT22 gives a new reading every 2 s at most; asking faster returns the last one.
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>
#include <DHT.h>
#include <math.h>

/**
 * @class TemperatureSensor
 * @brief Reads the temperature in Celsius and tracks the latest sample validity.
 */
class TemperatureSensor {
public:
    /**
     * @brief Builds the sensor.
     * @param dataPin Pin wired to the DHT22 DATA leg.
     */
    explicit TemperatureSensor(int dataPin)
        : dht(dataPin, DHT22),
          temperatureCelsius(NAN),
          humidityPercent(NAN), lastAttemptMs(0), hasAttempted(false) {}

    /** @brief Starts the sensor. Call it once from setup(). */
    void begin() {
        dht.begin();
        temperatureCelsius = humidityPercent = NAN;
        lastAttemptMs = millis();
        hasAttempted = false;
    }

    /**
     * @brief Takes a new reading.
     * @return false when the sensor did not answer (loose wire, wrong pin).
     */
    bool read() {
        // Commands/BOOT may request updates faster than the physical sample rate.
        if (millis() - lastAttemptMs < MIN_READ_INTERVAL_MS) {
            return hasAttempted && isfinite(temperatureCelsius) && isfinite(humidityPercent);
        }
        lastAttemptMs = millis();
        hasAttempted = true;
        float newTemperature = dht.readTemperature(false);
        float newHumidity = dht.readHumidity();

        if (!isfinite(newTemperature) || !isfinite(newHumidity) || newHumidity < 0.0f || newHumidity > 100.0f) {
            temperatureCelsius = humidityPercent = NAN;
            return false;
        }

        temperatureCelsius = newTemperature;
        humidityPercent = newHumidity;
        return true;
    }

    /**
     * @brief Temperature from the latest sampling attempt (cached for at most 2 s).
     * @return Degrees Celsius, or NAN after a failed reading or before the first sample.
     */
    float getTemperatureCelsius() const { return temperatureCelsius; }

    float getHumidityPercent() const { return humidityPercent; }

private:
    static constexpr unsigned long MIN_READ_INTERVAL_MS = 2000;
    DHT dht;
    float temperatureCelsius;
    float humidityPercent;
    unsigned long lastAttemptMs;
    bool hasAttempted;
};

#endif // TEMPERATURE_SENSOR_H
