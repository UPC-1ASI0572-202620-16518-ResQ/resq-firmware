#ifndef SAFETY_STATUS_RECORD_H
#define SAFETY_STATUS_RECORD_H

/**
 * @file SafetyStatusRecord.h
 * @brief Snapshot that the ResQ device sends to ThingSpeak on every publish.
 * @author ResQ IoT development team
 */

#include <Arduino.h>
#include <math.h>

/**
 * @class SafetyStatusRecord
 * @brief Values of the sensors and of the alarm at one moment (one ThingSpeak entry).
 */
class SafetyStatusRecord {
public:
    /**
     * @brief Builds the record.
     * @param gasRawLevel MQ-2 raw ADC value (Field 1).
     * @param temperatureCelsius Temperature in Celsius (Field 2).
     * @param rssiDbm Wi-Fi signal strength (Field 3).
     * @param isAlarmOn true when the risk is DANGER (Field 4).
     * @param isMotionDetected true when the PIR sees someone (Field 5).
     * @param humidityPercent Relative humidity (Field 6).
     * @param statusMessage Text for the ThingSpeak status, or nullptr to send none.
     */
    SafetyStatusRecord(int gasRawLevel, float temperatureCelsius, int rssiDbm, bool isAlarmOn,
                       bool isMotionDetected, float humidityPercent, const char* statusMessage)
        : gasRawLevel(gasRawLevel), temperatureCelsius(temperatureCelsius), rssiDbm(rssiDbm),
          isAlarmOn(isAlarmOn), isMotionDetected(isMotionDetected), humidityPercent(humidityPercent), statusMessage(statusMessage) {}

    int getGasRawLevel() const { return gasRawLevel; }
    float getTemperatureCelsius() const { return temperatureCelsius; }
    float getHumidityPercent() const { return humidityPercent; }
    bool hasValidMeasurements() const {
        return isfinite(temperatureCelsius) && isfinite(humidityPercent)
               && humidityPercent >= 0.0f && humidityPercent <= 100.0f;
    }
    int getRssiDbm() const { return rssiDbm; }
    int getAlarmFlag() const { return isAlarmOn ? 1 : 0; }
    int getMotionFlag() const { return isMotionDetected ? 1 : 0; }
    bool hasStatusMessage() const { return statusMessage != nullptr; }
    const char* getStatusMessage() const { return statusMessage; }

private:
    int gasRawLevel;
    float temperatureCelsius;
    int rssiDbm;
    bool isAlarmOn;
    bool isMotionDetected;
    float humidityPercent;
    const char* statusMessage;
};

#endif // SAFETY_STATUS_RECORD_H
