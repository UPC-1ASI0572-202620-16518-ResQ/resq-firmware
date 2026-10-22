#ifndef THINGSPEAK_PUBLISHER_H
#define THINGSPEAK_PUBLISHER_H

/**
 * @file ThingSpeakPublisher.h
 * @brief Sends one SafetyStatusRecord to a ThingSpeak channel (MathWorks ThingSpeak library).
 *
 * Channel fields:
 *   Field 1 Gas MQ-2 (raw) | Field 2 Temperatura (C) | Field 3 RSSI (dBm)
 *   Field 4 Alarma (0/1)   | Field 5 Movimiento (0/1)
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include "SafetyStatusRecord.h"

/**
 * @class ThingSpeakPublisher
 * @brief Fills the channel fields and writes them in a single request.
 */
class ThingSpeakPublisher {
public:
    static constexpr unsigned int FIELD_GAS = 1;
    static constexpr unsigned int FIELD_TEMPERATURE = 2;
    static constexpr unsigned int FIELD_RSSI = 3;
    static constexpr unsigned int FIELD_ALARM = 4;
    static constexpr unsigned int FIELD_MOTION = 5;
    // Result codes explained in part E of the lab guide
    static constexpr int RESULT_OK = 200;
    static constexpr int RESULT_BAD_API_KEY = 404;
    static constexpr int RESULT_TOO_SOON = -401;
    static constexpr int RESULT_NO_CONNECTION = -301;

    /**
     * @brief Builds the publisher.
     * @param channelId ThingSpeak Channel ID.
     * @param writeApiKey ThingSpeak Write API Key.
     */
    ThingSpeakPublisher(unsigned long channelId, const char* writeApiKey)
        : channelId(channelId), writeApiKey(writeApiKey) {}

    /** @brief Starts the ThingSpeak library. Call it once, after the Wi-Fi. */
    void begin() { ThingSpeak.begin(networkClient); }

    /**
     * @brief Writes every field of the record (and its status message, if any).
     * @param record Values to send.
     * @return ThingSpeak result code (200 = OK).
     */
    int publish(const SafetyStatusRecord& record) {
        ThingSpeak.setField(FIELD_GAS, record.getGasRawLevel());
        ThingSpeak.setField(FIELD_TEMPERATURE, record.getTemperatureCelsius());
        ThingSpeak.setField(FIELD_RSSI, record.getRssiDbm());
        ThingSpeak.setField(FIELD_ALARM, record.getAlarmFlag());
        ThingSpeak.setField(FIELD_MOTION, record.getMotionFlag());
        if (record.hasStatusMessage()) {
            ThingSpeak.setStatus(record.getStatusMessage());
        }
        return ThingSpeak.writeFields(channelId, writeApiKey);
    }

    /**
     * @brief Short explanation of a result code, for the Serial Monitor.
     * @param resultCode Code returned by publish().
     * @return Text in Spanish.
     */
    static const char* describeResult(int resultCode) {
        switch (resultCode) {
            case RESULT_OK: return "envio correcto";
            case RESULT_BAD_API_KEY: return "Write API Key incorrecta: revisa secrets.h";
            case RESULT_TOO_SOON: return "enviado antes de 15 s";
            case RESULT_NO_CONNECTION: return "sin conexion a ThingSpeak: revisa hotspot";
            default: return "error, revisa Wi-Fi y secrets.h";
        }
    }

private:
    WiFiClient networkClient;
    unsigned long channelId;
    const char* writeApiKey;
};

#endif // THINGSPEAK_PUBLISHER_H
