#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>

// Conexion asincrona para que la alarma local no dependa de la red.
// Evita llamadas WiFi.reconnect() cuando el driver sigue asociandose.
class WiFiConnection {
public:
    static constexpr unsigned long CONNECT_TIMEOUT_MS = 25000UL;
    static constexpr unsigned long RETRY_INTERVAL_MS = 25000UL;

    WiFiConnection(const char* networkName, const char* networkPassword)
        : networkName(networkName), networkPassword(networkPassword) {}

    bool begin() {
        WiFi.mode(WIFI_STA);
        WiFi.setAutoReconnect(false); // Reintentos controlados por poll().
        // Usar potencia normal para mejorar estabilidad con hotspot de iPhone.
        WiFi.setTxPower(WIFI_POWER_19_5dBm);
        startAttempt();
        return isConnected();
    }

    void poll() {
        const unsigned long now=millis();
        const wl_status_t status=WiFi.status();

        if(status==WL_CONNECTED) {
            if(!wasConnected) {
                wasConnected=true;
                Serial.println("[WiFi] Conectado");
                printDetails();
            }
            return;
        }

        if(wasConnected) {
            wasConnected=false;
            Serial.printf("[WiFi] Conexion perdida. Estado=%d\n", static_cast<int>(status));
            lastAttemptMs=now;
            return;
        }

        // No volver a conectar mientras un intento anterior puede estar activo.
        // Si se agota el plazo, reiniciar la asociacion y dejar diagnostico.
        if(now-lastAttemptMs < RETRY_INTERVAL_MS) return;
        Serial.printf("[WiFi] Reintento; estado anterior=%d\n",static_cast<int>(status));
        WiFi.disconnect(false,false);
        startAttempt();
    }

    bool isConnected() const { return WiFi.status()==WL_CONNECTED; }
    void reconnect() { poll(); } // Seguro: no fuerza intentos superpuestos.
    int getRssiDbm() const { return isConnected()?WiFi.RSSI():0; }

    void printDetails() const {
        Serial.printf("[WiFi] IP %s | RSSI %d dBm | MAC %s\n",
            WiFi.localIP().toString().c_str(),
            getRssiDbm(),
            WiFi.macAddress().c_str());
    }

private:
    void startAttempt() {
        lastAttemptMs=millis();
        Serial.printf("[WiFi] Intentando conectar a SSID: %s\n", networkName);
        // No imprimir nunca la clave de la red.
        WiFi.begin(networkName,networkPassword);
    }

    const char* networkName;
    const char* networkPassword;
    unsigned long lastAttemptMs=0;
    bool wasConnected=false;
};
#endif
