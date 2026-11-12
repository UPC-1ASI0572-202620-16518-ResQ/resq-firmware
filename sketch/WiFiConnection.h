#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

/**
 * @file WiFiConnection.h
 * @brief Connects the ESP32 to a 2.4 GHz Wi-Fi network with a time limit,
 *        so the alarm keeps working even without internet.
 * @author ResQ IoT development team
 */

#include <Arduino.h>
#include <WiFi.h>

/**
 * @class WiFiConnection
 * @brief Wi-Fi station: connect, report IP/RSSI/MAC and reconnect.
 */
class WiFiConnection {
public:
    static constexpr unsigned long CONNECT_TIMEOUT_MS = 15000;
    static constexpr unsigned long CONNECT_RETRY_DELAY_MS = 250;

    /**
     * @brief Builds the connection.
     * @param networkName Wi-Fi SSID (must be 2.4 GHz, no login page).
     * @param networkPassword Wi-Fi password.
     */
    WiFiConnection(const char* networkName, const char* networkPassword)
        : networkName(networkName), networkPassword(networkPassword) {}

    /** @brief Starts association without waiting (local alarms remain responsive). */
    bool begin() {
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);
        WiFi.begin(networkName,networkPassword);
        lastAttemptMs=millis();
        return isConnected();
    }

    void poll() {
        if (!isConnected() && millis()-lastAttemptMs>=10000UL) {
            lastAttemptMs=millis();
            WiFi.reconnect();
        }
    }

    /** @return true while the ESP32 is connected to the network. */
    bool isConnected() const { return WiFi.status() == WL_CONNECTED; }

    /** @brief Asks the ESP32 to connect again (does not wait). */
    void reconnect() { WiFi.reconnect(); }

    /** @return Signal strength in dBm (0 when offline). */
    int getRssiDbm() const { return isConnected() ? WiFi.RSSI() : 0; }

    /** @brief Prints IP, RSSI and MAC (evidence E2 of the lab). */
    void printDetails() const {
        Serial.printf("[WiFi] IP %s | RSSI %d dBm | MAC %s\n",
                      WiFi.localIP().toString().c_str(), getRssiDbm(), WiFi.macAddress().c_str());
    }

private:
    const char* networkName;
    const char* networkPassword;
    unsigned long lastAttemptMs=0;
};

#endif // WIFI_CONNECTION_H
