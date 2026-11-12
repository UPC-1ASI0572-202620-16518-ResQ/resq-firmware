#ifndef RESQ_MONITOR_DEVICE_H
#define RESQ_MONITOR_DEVICE_H

#include <Arduino.h>
#include <math.h>
#include "ResQConfig.h"
#include "DemoController.h"
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

enum class RiskLevel { WARMING_UP, NORMAL, CAUTION, DANGER, SENSOR_ERROR };
enum class LabRoute { ROUTE_1_SIMULATED=1, ROUTE_2_REAL_SENSOR=2, ROUTE_3_REAL_WITH_ALARM=3 };
struct RealSensorSet { bool isGasReal; bool isTemperatureReal; bool isMotionReal; };
struct ResQPinMap {
    int gasAnalogPin, temperatureDataPin, motionPin;
    int greenLedPin, redLedPin, boardLedPin, buzzerPin;
    int bootButtonPin, displaySdaPin, displaySclPin;
    int yellowLedPin = ResQConfig::PIN_LED_AMARILLO;
};
struct CloudCredentials {
    const char* wifiSsid;
    const char* wifiPassword;
    unsigned long channelId;
    const char* writeApiKey;
};

// ResQ 2.0: sensado real, overlay de demo, controles locales y publicacion.
// La alarma local no depende de la conexion a Internet.
class ResQMonitorDevice {
public:
    static constexpr uint8_t DISPLAY_I2C_ADDRESS=0x3C;
    ResQMonitorDevice(const ResQPinMap& pins, LabRoute route,
                      const RealSensorSet& realSensors, const CloudCredentials& credentials);
    void begin();
    void update();
private:
    GasDetector gasDetector;
    TemperatureSensor temperatureSensor;
    MotionDetector motionDetector;
    SensorSimulator sensorSimulator;
    PushButton bootButton;
    SerialConsole serialConsole;
    StatusLed greenLed, redLed, yellowLed, boardLed;
    AlarmBuzzer alarmBuzzer;
    OledStatusDisplay statusDisplay;
    WiFiConnection wifiConnection;
    ThingSpeakPublisher thingSpeakPublisher;
    DemoController demo;
    LabRoute route;
    RealSensorSet realSensors;

    float realTemperature=NAN, realHumidity=NAN;
    int realGas=0;
    bool realMotion=false, realDhtOk=false;
    unsigned long motionHoldUntil=0;
    bool pendingMotionCloud=false;

    float temperatureCelsius=NAN, humidityPercent=NAN;
    int gasRawLevel=0;
    bool isMotionDetected=false, isReadingValid=false;
    bool gasDanger=false, ambientCaution=false, ambientDanger=false;
    bool realDanger=false, isAlarmSilenced=false;
    bool oledOk=false, hadAlert=false;
    bool gasLatch=false, tempLatch=false, humidityLatch=false;
    int lastPublishResult=0;
    unsigned long bootMs=0, lastReadingMs=0, lastOutputMs=0, lastUiMs=0, lastPublishMs=0, lastCloudOkMs=0;
    enum class Page { HOME, NETWORK, DIAGNOSTICS };
    Page page=Page::HOME;

    bool usesRealGas() const;
    bool usesRealTemperature() const;
    bool usesRealMotion() const;
    bool usesAlarmLogic() const;
    void handleButton(ButtonEvent event);
    void handleConsoleCommand(const ConsoleCommand& command);
    void takeReadings();
    void evaluateState();
    void applyOutputs();
    void refreshDisplay();
    void publishToCloud();
    void printHelp() const;
    bool pirReady() const;
    bool gasReady() const;
    const char* alertName() const;
};
#endif
