#ifndef SENSOR_SIMULATOR_H
#define SENSOR_SIMULATOR_H

/**
 * @file SensorSimulator.h
 * @brief Lab 2 Route 1: controlled simulation of the sensors that are not wired yet.
 *
 * Each value moves by itself with a little noise around a base value. The student
 * controls the base values from the Serial Monitor (G=, T=, M=) and the BOOT button
 * opens or closes a simulated gas leak.
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>
#include "GasDetector.h"

/**
 * @class SensorSimulator
 * @brief Produces realistic readings for the MQ-2, the DHT22 and the PIR.
 */
class SensorSimulator {
public:
    static constexpr int DEFAULT_GAS_BASE_RAW = 600;              ///< clean air
    static constexpr float DEFAULT_TEMPERATURE_BASE_CELSIUS = 24.0f;
    static constexpr int GAS_LEAK_EXTRA_RAW = 2200;               ///< leak pushes 600 to ~2800 (DANGER)
    static constexpr int GAS_NOISE_RAW = 40;                      ///< +/- noise on every gas reading
    static constexpr int TEMPERATURE_NOISE_TENTHS = 3;            ///< +/- 0.3 C noise
    static constexpr float TENTHS_PER_DEGREE = 10.0f;

    SensorSimulator()
        : gasBaseRaw(DEFAULT_GAS_BASE_RAW),
          temperatureBaseCelsius(DEFAULT_TEMPERATURE_BASE_CELSIUS),
          isMotionDetected(false),
          isGasLeakOpen(false) {}

    /** @brief Seeds the random generator with the ESP32 hardware RNG. */
    void begin() { randomSeed(esp_random()); }

    /** @param newBaseRaw New gas base value (0..4095), typed as G=valor. */
    void setGasBase(int newBaseRaw) {
        gasBaseRaw = constrain(newBaseRaw, GasDetector::MIN_RAW_LEVEL, GasDetector::MAX_RAW_LEVEL);
    }

    /** @param newBaseCelsius New temperature base value, typed as T=valor. */
    void setTemperatureBase(float newBaseCelsius) { temperatureBaseCelsius = newBaseCelsius; }

    /** @param isDetected Simulated PIR state, typed as M=1 or M=0. */
    void setMotion(bool isDetected) { isMotionDetected = isDetected; }

    /**
     * @brief Opens or closes the simulated gas leak (BOOT button).
     * @return true if the leak is now open.
     */
    bool toggleGasLeak() {
        isGasLeakOpen = !isGasLeakOpen;
        return isGasLeakOpen;
    }

    /** @return Simulated MQ-2 raw value, with noise. */
    int readGasLevel() const {
        int leakRaw = isGasLeakOpen ? GAS_LEAK_EXTRA_RAW : 0;
        int noiseRaw = random(-GAS_NOISE_RAW, GAS_NOISE_RAW + 1);
        return constrain(gasBaseRaw + leakRaw + noiseRaw, GasDetector::MIN_RAW_LEVEL, GasDetector::MAX_RAW_LEVEL);
    }

    /** @return Simulated temperature in Celsius, with noise. */
    float readTemperatureCelsius() const {
        int noiseTenths = random(-TEMPERATURE_NOISE_TENTHS, TEMPERATURE_NOISE_TENTHS + 1);
        return temperatureBaseCelsius + noiseTenths / TENTHS_PER_DEGREE;
    }

    /** @return Simulated relative humidity; used only when the DHT is simulated. */
    float readHumidityPercent() const { return 50.0f; }

    /** @return Simulated PIR state. */
    bool readMotion() const { return isMotionDetected; }

    int getGasBaseRaw() const { return gasBaseRaw; }
    float getTemperatureBaseCelsius() const { return temperatureBaseCelsius; }
    bool getIsGasLeakOpen() const { return isGasLeakOpen; }

private:
    int gasBaseRaw;
    float temperatureBaseCelsius;
    bool isMotionDetected;
    bool isGasLeakOpen;
};

#endif // SENSOR_SIMULATOR_H
