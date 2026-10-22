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

/**
 * @class TemperatureSensor
 * @brief Reads the temperature in Celsius and remembers the last valid value.
 */
class TemperatureSensor {
public:
    /**
     * @brief Builds the sensor.
     * @param dataPin Pin wired to the DHT22 DATA leg.
     */
    explicit TemperatureSensor(int dataPin)
        : dht(dataPin, DHT22), temperatureCelsius(NAN) {}

    /** @brief Starts the sensor. Call it once from setup(). */
    void begin() { dht.begin(); }

    /**
     * @brief Takes a new reading.
     * @return false when the sensor did not answer (loose wire, wrong pin).
     */
    bool read() {
        float newTemperature = dht.readTemperature();
        if (isnan(newTemperature)) return false;
        temperatureCelsius = newTemperature;
        return true;
    }

    /**
     * @brief Last valid temperature.
     * @return Degrees Celsius, or NAN if there was never a valid reading.
     */
    float getTemperatureCelsius() const { return temperatureCelsius; }

private:
    DHT dht;
    float temperatureCelsius;
};

#endif // TEMPERATURE_SENSOR_H
