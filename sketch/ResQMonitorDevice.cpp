/**
 * @file ResQMonitorDevice.cpp
 * @brief Implements ResQMonitorDevice.
 * @author ResQ IoT development team
 */

#include "ResQMonitorDevice.h"

namespace {
constexpr char DISPLAY_TITLE[] = "RESQ";
constexpr char SIMULATED_TAG[] = "SIM";
constexpr char REAL_TAG[] = "REAL";
constexpr int DISPLAY_READINGS_LINE = 0;
constexpr int DISPLAY_MOTION_LINE = 1;
constexpr int DISPLAY_STATUS_LINE = 2;
constexpr int DISPLAY_CLOUD_LINE = 3;
constexpr int BLINK_PHASE_COUNT = 2;
constexpr int NO_PUBLISH_YET = 0;
}

ResQMonitorDevice::ResQMonitorDevice(const ResQPinMap& pins, LabRoute route, const RealSensorSet& realSensors,
                                     const CloudCredentials& credentials)
    : gasDetector(pins.gasAnalogPin, GAS_WARM_UP_TIME_MS),
      temperatureSensor(pins.temperatureDataPin),
      motionDetector(pins.motionPin),
      bootButton(pins.bootButtonPin),
      greenLed(pins.greenLedPin),
      redLed(pins.redLedPin),
      boardLed(pins.boardLedPin),
      alarmBuzzer(pins.buzzerPin),
      statusDisplay(DISPLAY_I2C_ADDRESS, pins.displaySdaPin, pins.displaySclPin, DISPLAY_TITLE),
      wifiConnection(credentials.wifiSsid, credentials.wifiPassword),
      thingSpeakPublisher(credentials.channelId, credentials.writeApiKey),
      route(route),
      realSensors(realSensors),
      gasRawLevel(GasDetector::MIN_RAW_LEVEL),
      temperatureCelsius(NAN),
      isMotionDetected(false),
      isReadingValid(false),
      riskLevel(RiskLevel::NORMAL),
      isAlarmSilenced(false),
      lastPublishResult(NO_PUBLISH_YET),
      lastReadingMs(0),
      lastOutputRefreshMs(0),
      lastPublishMs(0) {}

void ResQMonitorDevice::begin() {
    gasDetector.begin();
    temperatureSensor.begin();
    motionDetector.begin();
    sensorSimulator.begin();
    bootButton.begin();
    greenLed.begin();
    redLed.begin();
    boardLed.begin();
    alarmBuzzer.begin();

    if (!statusDisplay.begin()) {
        Serial.println("[OLED] No responde (revisa cables o prueba 0x3D); el resto sigue funcionando.");
    }
    bool isAnySensorReal = usesRealGas() || usesRealTemperature() || usesRealMotion();
    statusDisplay.setTag(isAnySensorReal ? REAL_TAG : SIMULATED_TAG);
    statusDisplay.setLine(DISPLAY_STATUS_LINE, "Conectando Wi-Fi...");
    statusDisplay.render();

    Serial.print("[WiFi] Conectando");
    if (wifiConnection.begin()) {
        wifiConnection.printDetails();
    } else {
        Serial.println("[WiFi] Sin conexion: la alarma funciona igual y se reintenta en cada envio.");
    }
    thingSpeakPublisher.begin();

    Serial.printf("[Lab2] Ruta %d | Gas: %s | Temperatura: %s | Movimiento: %s\n", static_cast<int>(route),
                  sourceText(usesRealGas()), sourceText(usesRealTemperature()), sourceText(usesRealMotion()));
    printHelp();
    takeReadings();
}

void ResQMonitorDevice::update() {
    ConsoleCommand command = serialConsole.poll();
    if (command.type != ConsoleCommandType::NONE) handleConsoleCommand(command);
    if (bootButton.wasPressed()) handleBootButton();

    if (millis() - lastReadingMs >= SENSOR_READ_INTERVAL_MS) takeReadings();
    if (millis() - lastOutputRefreshMs >= OUTPUT_REFRESH_INTERVAL_MS) {
        lastOutputRefreshMs = millis();
        applyOutputs();
    }
    if (millis() - lastPublishMs >= PUBLISH_INTERVAL_MS) publishToCloud();
}

bool ResQMonitorDevice::usesRealGas() const {
    return route != LabRoute::ROUTE_1_SIMULATED && realSensors.isGasReal;
}

bool ResQMonitorDevice::usesRealTemperature() const {
    return route != LabRoute::ROUTE_1_SIMULATED && realSensors.isTemperatureReal;
}

bool ResQMonitorDevice::usesRealMotion() const {
    return route != LabRoute::ROUTE_1_SIMULATED && realSensors.isMotionReal;
}

bool ResQMonitorDevice::usesAlarmLogic() const {
    return route != LabRoute::ROUTE_2_REAL_SENSOR;
}

void ResQMonitorDevice::handleConsoleCommand(const ConsoleCommand& command) {
    switch (command.type) {
        case ConsoleCommandType::SET_GAS:
            sensorSimulator.setGasBase(static_cast<int>(command.value));
            break;
        case ConsoleCommandType::SET_TEMPERATURE:
            sensorSimulator.setTemperatureBase(command.value);
            break;
        case ConsoleCommandType::SET_MOTION:
            sensorSimulator.setMotion(command.value != 0.0f);
            break;
        case ConsoleCommandType::SILENCE:
            isAlarmSilenced = true;
            Serial.println("[App] Alarma silenciada (el buzzer calla, Field 4 sigue en 1)");
            return;
        case ConsoleCommandType::UNSILENCE:
            isAlarmSilenced = false;
            Serial.println("[App] Alarma reactivada");
            return;
        case ConsoleCommandType::HELP:
            printHelp();
            return;
        default:
            return;
    }
    Serial.printf("[SIM] G=%d T=%.1f M=%d | fuga BOOT: %s\n", sensorSimulator.getGasBaseRaw(),
                  sensorSimulator.getTemperatureBaseCelsius(), sensorSimulator.readMotion() ? 1 : 0,
                  sensorSimulator.getIsGasLeakOpen() ? "ON" : "OFF");
    takeReadings();   // show the change right away, without waiting 2 s
}

void ResQMonitorDevice::handleBootButton() {
    bool isLeakOpen = sensorSimulator.toggleGasLeak();
    Serial.printf("[SIM] BOOT: fuga de gas %s\n", isLeakOpen ? "ON" : "OFF");
    if (usesRealGas()) Serial.println("[SIM] (el gas es real en esta ruta, la fuga simulada no se usa)");
    takeReadings();
}

void ResQMonitorDevice::takeReadings() {
    lastReadingMs = millis();
    isReadingValid = true;

    gasRawLevel = usesRealGas() ? gasDetector.read() : sensorSimulator.readGasLevel();

    if (usesRealTemperature()) {
        if (!temperatureSensor.read()) {
            Serial.println("[DHT] Error de lectura");
            isReadingValid = false;
        }
        temperatureCelsius = temperatureSensor.getTemperatureCelsius();
    } else {
        temperatureCelsius = sensorSimulator.readTemperatureCelsius();
    }

    isMotionDetected = usesRealMotion() ? motionDetector.read() : sensorSimulator.readMotion();

    RiskLevel newRiskLevel = evaluateRiskLevel();
    if (newRiskLevel != riskLevel) {
        Serial.printf("[Riesgo] %s -> %s | gas %d | temp %.1f C\n", riskLevelToText(riskLevel),
                      riskLevelToText(newRiskLevel), gasRawLevel, temperatureCelsius);
        riskLevel = newRiskLevel;
        if (riskLevel == RiskLevel::NORMAL) isAlarmSilenced = false;   // the next alarm sounds again
    }
    applyOutputs();
    refreshDisplay();
}

RiskLevel ResQMonitorDevice::evaluateRiskLevel() const {
    if (usesRealGas() && !gasDetector.isWarmedUp()) return RiskLevel::WARMING_UP;
    // A NAN temperature (failed DHT22 reading) makes both comparisons false
    if (gasRawLevel >= GAS_DANGER_RAW_LEVEL || temperatureCelsius >= TEMPERATURE_DANGER_CELSIUS) {
        return RiskLevel::DANGER;
    }
    if (gasRawLevel >= GAS_CAUTION_RAW_LEVEL || temperatureCelsius >= TEMPERATURE_CAUTION_CELSIUS) {
        return RiskLevel::CAUTION;
    }
    return RiskLevel::NORMAL;
}

bool ResQMonitorDevice::isAlarmOn() const {
    return usesAlarmLogic() && riskLevel == RiskLevel::DANGER;
}

void ResQMonitorDevice::applyOutputs() {
    if (!usesAlarmLogic()) {
        // Route 2: data only, the green LED just says "system on"
        greenLed.setOn(true);
        redLed.setOn(false);
        boardLed.setOn(false);
        alarmBuzzer.setSounding(false);
        return;
    }
    // Green: NORMAL. Red blinking: CAUTION or WARMING_UP. Red steady + buzzer: DANGER.
    const bool isBlinkPhaseOn = (millis() / CAUTION_BLINK_HALF_PERIOD_MS) % BLINK_PHASE_COUNT;
    bool isRedLedOn = false;
    if (riskLevel == RiskLevel::DANGER) isRedLedOn = true;
    else if (riskLevel != RiskLevel::NORMAL) isRedLedOn = isBlinkPhaseOn;

    greenLed.setOn(riskLevel == RiskLevel::NORMAL);
    redLed.setOn(isRedLedOn);
    boardLed.setOn(isAlarmOn());
    alarmBuzzer.setSounding(isAlarmOn() && !isAlarmSilenced);
}

void ResQMonitorDevice::refreshDisplay() {
    char line[OledStatusDisplay::LINE_SIZE];
    statusDisplay.clearLines();

    snprintf(line, sizeof(line), "Gas:%d T:%.1fC", gasRawLevel, temperatureCelsius);
    statusDisplay.setLine(DISPLAY_READINGS_LINE, line);

    snprintf(line, sizeof(line), "Mov:%s RSSI:%d", isMotionDetected ? "SI" : "NO", wifiConnection.getRssiDbm());
    statusDisplay.setLine(DISPLAY_MOTION_LINE, line);

    if (riskLevel == RiskLevel::WARMING_UP) {
        snprintf(line, sizeof(line), "CALENTANDO %lu s", gasDetector.getRemainingWarmUpSeconds());
    } else if (isAlarmOn() && isAlarmSilenced) {
        snprintf(line, sizeof(line), "ALERTA (silenciada)");
    } else {
        snprintf(line, sizeof(line), "%s", buildStatusMessage());
    }
    statusDisplay.setLine(DISPLAY_STATUS_LINE, line);

    if (lastPublishResult == NO_PUBLISH_YET) {
        snprintf(line, sizeof(line), "Nube: esperando...");
    } else {
        snprintf(line, sizeof(line), "Nube: %d %s", lastPublishResult,
                 lastPublishResult == ThingSpeakPublisher::RESULT_OK ? "OK" : "ERROR");
    }
    statusDisplay.setLine(DISPLAY_CLOUD_LINE, line);

    statusDisplay.render();
}

const char* ResQMonitorDevice::buildStatusMessage() const {
    if (!usesAlarmLogic()) return "SOLO MONITOREO";
    switch (riskLevel) {
        case RiskLevel::WARMING_UP: return "CALENTANDO MQ-2";
        case RiskLevel::DANGER:
            return gasRawLevel >= GAS_DANGER_RAW_LEVEL ? "ALERTA DE GAS" : "ALERTA TEMPERATURA";
        case RiskLevel::CAUTION: return "PRECAUCION";
        default: return "ESTADO: NORMAL";
    }
}

void ResQMonitorDevice::publishToCloud() {
    lastPublishMs = millis();
    if (!wifiConnection.isConnected()) {
        Serial.println("[WiFi] Reconectando...");
        wifiConnection.reconnect();
        return;
    }
    if (!isReadingValid) return;   // lab guide: if the reading fails, nothing is sent

    // Routes 1 and 3 also send a status message (ThingSpeak > Channel Status Updates)
    SafetyStatusRecord record(gasRawLevel, temperatureCelsius, wifiConnection.getRssiDbm(), isAlarmOn(),
                              isMotionDetected, usesAlarmLogic() ? buildStatusMessage() : nullptr);
    lastPublishResult = thingSpeakPublisher.publish(record);
    Serial.printf("[TS] gas=%d temp=%.1f rssi=%d alarma=%d mov=%d -> codigo %d (%s)\n",
                  record.getGasRawLevel(), record.getTemperatureCelsius(), record.getRssiDbm(),
                  record.getAlarmFlag(), record.getMotionFlag(), lastPublishResult,
                  ThingSpeakPublisher::describeResult(lastPublishResult));
    refreshDisplay();
}

void ResQMonitorDevice::printHelp() const {
    Serial.println("[SIM] Comandos: G=valor (gas 0-4095)  T=valor (C)  M=1/M=0 (movimiento)");
    Serial.println("[SIM]           SILENCE / UNSILENCE (buzzer)  HELP | BOOT = fuga de gas on/off");
    Serial.printf("[SIM] Umbrales: gas %d precaucion / %d peligro | temp %.0f / %.0f C\n",
                  GAS_CAUTION_RAW_LEVEL, GAS_DANGER_RAW_LEVEL,
                  TEMPERATURE_CAUTION_CELSIUS, TEMPERATURE_DANGER_CELSIUS);
}

const char* ResQMonitorDevice::riskLevelToText(RiskLevel level) {
    switch (level) {
        case RiskLevel::WARMING_UP: return "WARMING_UP";
        case RiskLevel::DANGER: return "DANGER";
        case RiskLevel::CAUTION: return "CAUTION";
        default: return "NORMAL";
    }
}

const char* ResQMonitorDevice::sourceText(bool isReal) {
    return isReal ? "REAL" : "SIMULADO";
}
