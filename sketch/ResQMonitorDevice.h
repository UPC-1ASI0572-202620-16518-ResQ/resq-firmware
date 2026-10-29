#ifndef RESQ_MONITOR_DEVICE_H
#define RESQ_MONITOR_DEVICE_H

/**
 * @file ResQMonitorDevice.h
 * @brief ResQ for Lab 2: measures gas (MQ-2), temperature (DHT22) and motion (PIR),
 *        warns with the green/red LEDs, the buzzer and the OLED, and publishes to ThingSpeak.
 *
 * Every sensor can be real or simulated, so the same code covers the three routes
 * of the lab guide.
 *
 * @author ResQ IoT development team
 */

#include <Arduino.h>
#include "GasDetector.h"
#include "TemperatureSensor.h"
#include "MotionDetector.h"
#include "SensorSimulator.h"
#include "PushButton.h"
#include "SerialConsole.h"
#include "StatusLed.h"
#include "AlarmBuzzer.h"
#include "OledStatusDisplay.h"
#include "SafetyStatusRecord.h"
#include "WiFiConnection.h"
#include "ThingSpeakPublisher.h"

/** @brief Risk level of the room. */
enum class RiskLevel { WARMING_UP, NORMAL, CAUTION, DANGER, SENSOR_ERROR };

/** @brief Work routes of the lab guide (section 4). */
enum class LabRoute {
    ROUTE_1_SIMULATED = 1,          ///< every sensor simulated, with alarm, LED and status
    ROUTE_2_REAL_SENSOR = 2,        ///< real sensor(s), data only (alarm always 0)
    ROUTE_3_REAL_WITH_ALARM = 3     ///< real sensor(s) with alarm, LED and status message
};

/** @brief Which sensors are physically wired (only used in routes 2 and 3). */
struct RealSensorSet {
    bool isGasReal;
    bool isTemperatureReal;
    bool isMotionReal;
};

/** @brief ESP32 pins used by ResQ. */
struct ResQPinMap {
    int gasAnalogPin;        ///< MQ-2 AO (through the 10k/10k divider)
    int temperatureDataPin;  ///< DHT22 DATA
    int motionPin;           ///< PIR OUT
    int greenLedPin;         ///< green LED
    int redLedPin;           ///< red LED
    int boardLedPin;         ///< blue LED on the DevKit board (the guide's alarm LED)
    int buzzerPin;           ///< active buzzer "+"
    int bootButtonPin;       ///< BOOT button on the board
    int displaySdaPin;       ///< OLED SDA
    int displaySclPin;       ///< OLED SCL
};

/** @brief ThingSpeak and Wi-Fi credentials (from secrets.h). */
struct CloudCredentials {
    const char* wifiSsid;
    const char* wifiPassword;
    unsigned long channelId;
    const char* writeApiKey;
};

/**
 * @class ResQMonitorDevice
 * @brief Main device: owns every component and applies the rules.
 *
 * Usage: call begin() once in setup() and update() on every loop().
 */
class ResQMonitorDevice {
public:
    // Business rules (raw ADC values, demo only, not ppm)
    static constexpr int GAS_CAUTION_RAW_LEVEL = 1400;
    static constexpr int GAS_DANGER_RAW_LEVEL = 2500;
    static constexpr float TEMPERATURE_CAUTION_CELSIUS = 40.0f;
    static constexpr float TEMPERATURE_DANGER_CELSIUS = 50.0f;   ///< heat of a possible fire
    // Timing
    static constexpr unsigned long GAS_WARM_UP_TIME_MS = 20000;
    static constexpr unsigned long SENSOR_READ_INTERVAL_MS = 2000;   ///< DHT22 maximum rate
    static constexpr unsigned long OUTPUT_REFRESH_INTERVAL_MS = 250;
    static constexpr unsigned long CAUTION_BLINK_HALF_PERIOD_MS = 500;
    static constexpr unsigned long PUBLISH_INTERVAL_MS = 20000;      ///< free ThingSpeak: >= 15 s
    // Hardware
    static constexpr uint8_t DISPLAY_I2C_ADDRESS = 0x3C;

    /**
     * @brief Builds every component (nothing touches the hardware until begin()).
     * @param pins ESP32 pins of every component.
     * @param route Lab route to run.
     * @param realSensors Sensors that are wired (ignored in route 1).
     * @param credentials Wi-Fi and ThingSpeak data.
     */
    ResQMonitorDevice(const ResQPinMap& pins, LabRoute route, const RealSensorSet& realSensors,
                      const CloudCredentials& credentials);

    /** @brief Starts pins, screen, Wi-Fi and ThingSpeak. Call it once from setup(). */
    void begin();

    /** @brief Reads commands and the BOOT button, reads sensors, acts and publishes on time. Call it from loop(). */
    void update();

private:
    // Real sensors
    GasDetector gasDetector;
    TemperatureSensor temperatureSensor;
    MotionDetector motionDetector;
    // Simulation (route 1, or sensors not wired yet)
    SensorSimulator sensorSimulator;
    PushButton bootButton;
    SerialConsole serialConsole;
    // Actuators
    StatusLed greenLed;
    StatusLed redLed;
    StatusLed boardLed;
    AlarmBuzzer alarmBuzzer;
    OledStatusDisplay statusDisplay;
    // Cloud
    WiFiConnection wifiConnection;
    ThingSpeakPublisher thingSpeakPublisher;
    // Configuration
    LabRoute route;
    RealSensorSet realSensors;
    // State
    int gasRawLevel;
    float temperatureCelsius;
    float humidityPercent;
    bool isMotionDetected;
    bool isReadingValid;
    RiskLevel riskLevel;
    bool isAlarmSilenced;
    int lastPublishResult;
    unsigned long lastReadingMs;
    unsigned long lastOutputRefreshMs;
    unsigned long lastPublishMs;

    /** @return true if this sensor is read from the hardware (false = simulated). */
    bool usesRealGas() const;
    bool usesRealTemperature() const;
    bool usesRealMotion() const;

    /** @return false in route 2 (data only: no alarm, no status message). */
    bool usesAlarmLogic() const;

    /** @brief Applies a command typed in the Serial Monitor. */
    void handleConsoleCommand(const ConsoleCommand& command);

    /** @brief BOOT button: opens or closes the simulated gas leak. */
    void handleBootButton();

    /** @brief Reads every sensor (real or simulated) and updates the risk level. */
    void takeReadings();

    /** @return WARMING_UP, NORMAL, CAUTION, DANGER or SENSOR_ERROR from the latest readings. */
    RiskLevel evaluateRiskLevel() const;

    /** @return true when Field 4 (alarm) must be 1. */
    bool isAlarmOn() const;

    /** @brief Sets LEDs and buzzer for the current risk level. */
    void applyOutputs();

    /** @brief Writes the current status on the OLED screen. */
    void refreshDisplay();

    /** @return Short status text for ThingSpeak and the OLED. */
    const char* buildStatusMessage() const;

    /** @brief Sends the current values to ThingSpeak (or reconnects if offline). */
    void publishToCloud();

    /** @brief Prints the commands accepted in this route. */
    void printHelp() const;

    static const char* riskLevelToText(RiskLevel level);
    static const char* sourceText(bool isReal);
};

#endif // RESQ_MONITOR_DEVICE_H
