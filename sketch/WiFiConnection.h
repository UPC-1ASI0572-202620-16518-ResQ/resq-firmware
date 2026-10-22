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

    /**
     * @brief Tries to connect, waiting at most CONNECT_TIMEOUT_MS.
     * @return true when connected.
     */
    bool begin() {
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(true);
        WiFi.setTxPower(WIFI_POWER_8_5dBm);   // lab guide: lowers current peaks over USB
        WiFi.begin(networkName, networkPassword);
        unsigned long startMs = millis();
        while (!isConnected() && millis() - startMs < CONNECT_TIMEOUT_MS) {
            delay(CONNECT_RETRY_DELAY_MS);
            Serial.print(".");
        }
        Serial.println();
        return isConnected();
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
};

#endif // WIFI_CONNECTION_H
