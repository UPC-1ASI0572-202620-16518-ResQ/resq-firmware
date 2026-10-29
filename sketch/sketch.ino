/**
 * @file sketch.ino
 * @brief ResQ - Lab 2: ESP32 connectivity with ThingSpeak (Arduino IDE, ESP32 DevKit V1).
 *
 * Board: "DOIT ESP32 DEVKIT V1". Serial Monitor at 115200 baud with "New Line".
 * Credentials go in secrets.h (copy of secrets.example.h, never uploaded to GitHub).
 *
 * To change route, edit only LAB_ROUTE (and REAL_SENSORS for routes 2 and 3).
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>
#include "secrets.h"
#include "ResQMonitorDevice.h"

static const char PRODUCT_NAME[] = "ResQ";
static const char TEAM_NAME[] = "ResQ IoT development team";   // TODO: team members
static const unsigned long SERIAL_BAUD_RATE = 115200;
static const unsigned long SERIAL_START_DELAY_MS = 500;

// Route 1 = everything simulated (works today, without the sensors).
// Routes 2 and 3 = real sensors: mark below which ones are wired; the rest stay simulated.
static const LabRoute LAB_ROUTE = LabRoute::ROUTE_3_REAL_WITH_ALARM;
static const RealSensorSet REAL_SENSORS = {
    .isGasReal = true,
    .isTemperatureReal = true,
    .isMotionReal = false,
};

// OLED and LEDs: the pins the team already wired. Sensors and buzzer: the Cirkit design.
static const ResQPinMap RESQ_PINS = {
    .gasAnalogPin = 34,
    .temperatureDataPin = 4,
    .motionPin = 33,
    .greenLedPin = 23,
    .redLedPin = 22,
    .boardLedPin = 2,
    .buzzerPin = 27,
    .bootButtonPin = 0,
    .displaySdaPin = 21,
    .displaySclPin = 19,
};

static const CloudCredentials CLOUD = {
    .wifiSsid = WIFI_SSID,
    .wifiPassword = WIFI_PASS,
    .channelId = TS_CHANNEL,
    .writeApiKey = TS_WRITE_KEY,
};

static ResQMonitorDevice resqMonitor(RESQ_PINS, LAB_ROUTE, REAL_SENSORS, CLOUD);

/** @brief Prints product and team information. */
void printStartupBanner() {
    Serial.println("==========================================");
    Serial.printf("Product: %s - Lab 2 ThingSpeak\n", PRODUCT_NAME);
    Serial.printf("Team:    %s\n", TEAM_NAME);
    Serial.println("==========================================");
}

/** @brief Starts the Serial Monitor and the device. */
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(SERIAL_START_DELAY_MS);
    printStartupBanner();
    resqMonitor.begin();
}

/** @brief Lets the device do one step; all the logic lives in ResQMonitorDevice. */
void loop() {
    resqMonitor.update();
}
