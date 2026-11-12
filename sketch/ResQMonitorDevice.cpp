#include "ResQMonitorDevice.h"

namespace {
bool hysteresisHigh(float value, float threshold, float margin, bool previous) {
    return value >= (previous ? threshold-margin : threshold);
}
}

ResQMonitorDevice::ResQMonitorDevice(
    const ResQPinMap& p, LabRoute r, const RealSensorSet& sensors,
    const CloudCredentials& credentials)
    : gasDetector(p.gasAnalogPin, ResQConfig::GAS_WARMUP_MS),
      temperatureSensor(p.temperatureDataPin),
      motionDetector(p.motionPin), bootButton(p.bootButtonPin),
      greenLed(p.greenLedPin), redLed(p.redLedPin),
      yellowLed(p.yellowLedPin), boardLed(p.boardLedPin),
      alarmBuzzer(p.buzzerPin),
      statusDisplay(DISPLAY_I2C_ADDRESS,p.displaySdaPin,p.displaySclPin,"RESQ"),
      wifiConnection(credentials.wifiSsid, credentials.wifiPassword),
      thingSpeakPublisher(credentials.channelId, credentials.writeApiKey),
      route(r),realSensors(sensors) {}

bool ResQMonitorDevice::usesRealGas() const {
    return route!=LabRoute::ROUTE_1_SIMULATED && realSensors.isGasReal;
}
bool ResQMonitorDevice::usesRealTemperature() const {
    return route!=LabRoute::ROUTE_1_SIMULATED && realSensors.isTemperatureReal;
}
bool ResQMonitorDevice::usesRealMotion() const {
    return route!=LabRoute::ROUTE_1_SIMULATED && realSensors.isMotionReal;
}
bool ResQMonitorDevice::usesAlarmLogic() const {
    return route!=LabRoute::ROUTE_2_REAL_SENSOR;
}
bool ResQMonitorDevice::pirReady() const {
    return !usesRealMotion() || (millis()-bootMs>=ResQConfig::PIR_WARMUP_MS);
}
bool ResQMonitorDevice::gasReady() const {
    return !usesRealGas() || (millis()-bootMs>=ResQConfig::GAS_WARMUP_MS);
}
void ResQMonitorDevice::begin() {
    bootMs=millis();
    gasDetector.begin();
    if(usesRealTemperature()) temperatureSensor.begin();
    if(usesRealMotion()) motionDetector.begin();
    sensorSimulator.begin();
    bootButton.begin();
    greenLed.begin(); redLed.begin(); yellowLed.begin(); boardLed.begin();
    alarmBuzzer.begin();
    oledOk=statusDisplay.begin();
    statusDisplay.setTag("REAL");
    statusDisplay.setLine(0,"Iniciando sensores...");
    statusDisplay.setLine(1,"BOOT: cambiar pagina");
    statusDisplay.setLine(2,"Doble: menu DEMO");
    statusDisplay.setLine(3,"WiFi: conectando");
    statusDisplay.render();

    // Asociacion Wi-Fi iniciada sin bloquear el monitoreo local.
    wifiConnection.begin();
    thingSpeakPublisher.begin();
    Serial.println("[RESQ2] Inicio | GPIO PIR=33, LED amarillo=25 | BOOT interfaz");
    printHelp();
    takeReadings();
    applyOutputs();
    refreshDisplay();
}

void ResQMonitorDevice::update() {
    const unsigned long now=millis();
    const ConsoleCommand command=serialConsole.poll();
    if(command.type!=ConsoleCommandType::NONE) handleConsoleCommand(command);
    handleButton(bootButton.poll());
    const bool wasDemoActive=demo.active();
    demo.update(now);
    if(wasDemoActive && !demo.active()) takeReadings();
    wifiConnection.poll();

    if(now-lastReadingMs>=ResQConfig::SENSOR_INTERVAL_MS) takeReadings();
    if(now-lastOutputMs>=ResQConfig::UI_INTERVAL_MS) {
        lastOutputMs=now;
        applyOutputs();
        refreshDisplay();
    }
    if(now-lastPublishMs>=ResQConfig::PUBLISH_INTERVAL_MS) publishToCloud();
}

void ResQMonitorDevice::handleButton(ButtonEvent event) {
    if(event==ButtonEvent::NONE) return;
    if(event==ButtonEvent::DOUBLE) {
        demo.toggleMenu();
        Serial.printf("[UI] Menu demo: %s\n",demo.menuOpen()?"ABIERTO":"CERRADO");
    } else if(event==ButtonEvent::SHORT) {
        if(demo.menuOpen()) demo.next();
        else if(!demo.active())
            page=static_cast<Page>((static_cast<int>(page)+1)%3);
    } else if(event==ButtonEvent::LONG) {
        if(demo.menuOpen()) {
            demo.start(millis());
            Serial.printf("[DEMO] Inicio %s\n",demo.selectedName());
            lastPublishMs=millis()-ResQConfig::PUBLISH_INTERVAL_MS; // intenta enviar en proximo loop
        } else if(demo.active()) {
            demo.stop();
            Serial.println("[DEMO] Cancelado");
        }
    }
    takeReadings(); // aplica las muestras sintéticas inmediatamente al entrar/salir
    applyOutputs();
    refreshDisplay();
}

void ResQMonitorDevice::handleConsoleCommand(const ConsoleCommand& c) {
    switch(c.type) {
        case ConsoleCommandType::SET_GAS:
            if(usesRealGas()) Serial.println("[SIM] Gas real: G= ignorado");
            else sensorSimulator.setGasBase(static_cast<int>(c.value));
            break;
        case ConsoleCommandType::SET_TEMPERATURE:
            if(usesRealTemperature()) Serial.println("[SIM] DHT real: T= ignorado");
            else sensorSimulator.setTemperatureBase(c.value);
            break;
        case ConsoleCommandType::SET_MOTION:
            if(usesRealMotion()) Serial.println("[SIM] PIR real: M= ignorado");
            else sensorSimulator.setMotion(c.value!=0.0f);
            break;
        case ConsoleCommandType::SILENCE:
            isAlarmSilenced=true; break;
        case ConsoleCommandType::UNSILENCE:
            isAlarmSilenced=false; break;
        case ConsoleCommandType::HELP:
            printHelp(); break;
        default: break;
    }
    takeReadings();
}

void ResQMonitorDevice::takeReadings() {
    const unsigned long now=millis();
    lastReadingMs=now;

    realGas=usesRealGas()?gasDetector.read():sensorSimulator.readGasLevel();
    if(usesRealTemperature()) {
        realDhtOk=temperatureSensor.read();
        realTemperature=temperatureSensor.getTemperatureCelsius();
        realHumidity=temperatureSensor.getHumidityPercent();
    } else {
        realTemperature=sensorSimulator.readTemperatureCelsius();
        realHumidity=sensorSimulator.readHumidityPercent();
        realDhtOk=true;
    }
    const bool rawMotion=usesRealMotion()?motionDetector.read():sensorSimulator.readMotion();
    if(pirReady() && rawMotion) {
        motionHoldUntil=now+ResQConfig::MOTION_LATCH_MS;
        pendingMotionCloud=true;
    }
    realMotion=pirReady() && static_cast<int32_t>(motionHoldUntil-now)>0;

    // Medicion efectiva: real por defecto; escenario DEMO puede sustituir
    // una variable para atravesar exactamente el mismo evaluador y publicador.
    gasRawLevel=realGas;
    temperatureCelsius=realTemperature;
    humidityPercent=realHumidity;
    isMotionDetected=realMotion;
    if(demo.active()) {
        switch(demo.selected()) {
            case DemoScenario::GAS: gasRawLevel=ResQConfig::DEMO_GAS_ADC; break;
            case DemoScenario::TEMPERATURE: temperatureCelsius=ResQConfig::DEMO_TEMP_C; break;
            case DemoScenario::HUMIDITY: humidityPercent=ResQConfig::DEMO_HUM_PCT; break;
            case DemoScenario::MOTION:
                isMotionDetected=true;
                pendingMotionCloud=true;
                break;
        }
    }
    isReadingValid=isfinite(temperatureCelsius)&&isfinite(humidityPercent)
                   &&humidityPercent>=0.0f && humidityPercent<=100.0f;

    evaluateState();
    Serial.printf("[RESQ] Gas=%d ADC Temp=%.1f Hum=%.1f PIR=%d DEMO=%s DHT=%s\n",
       gasRawLevel,temperatureCelsius,humidityPercent,isMotionDetected?1:0,
       demo.active()?demo.selectedName():"OFF",isReadingValid?"OK":"ERROR");
}

void ResQMonitorDevice::evaluateState() {
    // Los eventos reales no pueden ser ocultados por la DEMO.
    realDanger=realGas>=ResQConfig::GAS_PELIGRO;
    const bool canUseGas=gasReady() || demo.active() || realDanger;
    gasLatch=canUseGas && hysteresisHigh(gasRawLevel,ResQConfig::GAS_PELIGRO,
                 ResQConfig::GAS_HISTERESIS,gasLatch);
    tempLatch=isfinite(temperatureCelsius) &&
                 hysteresisHigh(temperatureCelsius,ResQConfig::TEMP_PRECAUCION_C,
                     ResQConfig::TEMP_HISTERESIS_C,tempLatch);
    humidityLatch=isfinite(humidityPercent) &&
                 hysteresisHigh(humidityPercent,ResQConfig::HUM_PRECAUCION_PCT,
                     ResQConfig::HUM_HISTERESIS_PCT,humidityLatch);
    gasDanger=gasLatch || realDanger;
    ambientDanger=gasDanger ||
       (isfinite(temperatureCelsius)&&temperatureCelsius>=ResQConfig::TEMP_PELIGRO_C) ||
       (isfinite(humidityPercent)&&humidityPercent>=ResQConfig::HUM_PELIGRO_PCT);
    ambientCaution=gasDanger || tempLatch || humidityLatch ||
       (canUseGas && gasRawLevel>=ResQConfig::GAS_PRECAUCION) ||
       !isReadingValid || !gasReady();
}

void ResQMonitorDevice::applyOutputs() {
    if(!usesAlarmLogic()) {
        greenLed.setOn(isReadingValid);
        redLed.setOn(!isReadingValid);
        yellowLed.setOn(false);
        alarmBuzzer.setPattern(BuzzerPattern::SILENT);
        return;
    }
    const unsigned long now=millis();
    const bool redBlink=(now/ResQConfig::RED_BLINK_HALF_MS)%2==0;
    const bool yellowBlink=(now/ResQConfig::YELLOW_BLINK_HALF_MS)%2==0;
    const bool environmentalAlert=ambientCaution || ambientDanger;
    greenLed.setOn(!environmentalAlert);
    redLed.setOn(gasDanger || (environmentalAlert && redBlink));
    yellowLed.setOn(isMotionDetected && yellowBlink);
    boardLed.setOn(gasDanger);
    BuzzerPattern pattern=BuzzerPattern::SILENT;
    if(!isAlarmSilenced) {
        if(gasDanger) pattern=BuzzerPattern::GAS;
        else if(isMotionDetected) pattern=BuzzerPattern::MOTION;
    }
    alarmBuzzer.setPattern(pattern);
}

const char* ResQMonitorDevice::alertName() const {
    if(gasDanger) return "ALERTA GAS";
    if(!isReadingValid) return "ERROR DHT";
    if(tempLatch) return "TEMP ALTA";
    if(humidityLatch) return "HUMEDAD ALTA";
    if(isMotionDetected) return "MOVIMIENTO";
    if(!gasReady()) return "CALENTANDO MQ2";
    if(ambientCaution) return "PRECAUCION GAS";
    return "NORMAL";
}

void ResQMonitorDevice::refreshDisplay() {
    if(!oledOk) return;
    char line[OledStatusDisplay::LINE_SIZE];
    statusDisplay.clearLines();
    statusDisplay.setTag(demo.active()||demo.menuOpen()?"DEMO":"REAL");

    if(demo.menuOpen()) {
        statusDisplay.setTitle("MENU DEMO");
        snprintf(line,sizeof(line),"Prueba: %s",demo.selectedName());
        statusDisplay.setLine(0,line);
        statusDisplay.setLine(1,"Corto: siguiente");
        statusDisplay.setLine(2,"Largo: ejecutar");
        statusDisplay.setLine(3,"Doble: salir");
    } else if(gasDanger || isMotionDetected || tempLatch || humidityLatch || !isReadingValid) {
        statusDisplay.setTitle("ALERTA");
        statusDisplay.setLine(0,alertName());
        snprintf(line,sizeof(line),"Gas:%d T:%.1fC",gasRawLevel,temperatureCelsius);
        statusDisplay.setLine(1,line);
        snprintf(line,sizeof(line),"PIR:%s Hum:%.0f%%",isMotionDetected?"SI":"NO",humidityPercent);
        statusDisplay.setLine(2,line);
        statusDisplay.setLine(3,demo.active()?"MODO DEMOSTRACION":"ALARMA LOCAL");
    } else if(demo.active()) {
        statusDisplay.setTitle("MODO DEMO");
        statusDisplay.setLine(0,demo.selectedName());
        snprintf(line,sizeof(line),"Gas:%d T:%.1fC",gasRawLevel,temperatureCelsius);
        statusDisplay.setLine(1,line);
        snprintf(line,sizeof(line),"Hum:%.0f%% PIR:%d",humidityPercent,isMotionDetected?1:0);
        statusDisplay.setLine(2,line);
        statusDisplay.setLine(3,"ENVIANDO A NUBE");
    } else if(page==Page::HOME) {
        statusDisplay.setTitle("RESQ");
        snprintf(line,sizeof(line),"Gas:%d ADC",gasRawLevel);
        statusDisplay.setLine(0,line);
        snprintf(line,sizeof(line),"T:%.1fC H:%.1f%%",temperatureCelsius,humidityPercent);
        statusDisplay.setLine(1,line);
        snprintf(line,sizeof(line),"PIR:%s",isMotionDetected?"MOVIMIENTO":"SIN MOV.");
        statusDisplay.setLine(2,line);
        snprintf(line,sizeof(line),"ESTADO: %s",alertName());
        statusDisplay.setLine(3,line);
    } else if(page==Page::NETWORK) {
        statusDisplay.setTitle("RED / NUBE");
        statusDisplay.setLine(0,wifiConnection.isConnected()?"WiFi: CONECTADO":"WiFi: SIN RED");
        snprintf(line,sizeof(line),"RSSI: %d dBm",wifiConnection.getRssiDbm());
        statusDisplay.setLine(1,line);
        snprintf(line,sizeof(line),"ThingSpeak: %s",lastPublishResult==200?"OK":lastPublishResult==0?"ESPERA":"ERROR");
        statusDisplay.setLine(2,line);
        if(lastCloudOkMs>0) snprintf(line,sizeof(line),"Ultimo OK: %lus",(millis()-lastCloudOkMs)/1000UL);
        else snprintf(line,sizeof(line),"Sin envio confirmado");
        statusDisplay.setLine(3,line);
    } else {
        statusDisplay.setTitle("DIAGNOSTICO");
        statusDisplay.setLine(0,gasReady()?"MQ2: LISTO":"MQ2: CALENTANDO");
        statusDisplay.setLine(1,isReadingValid?"DHT22: OK":"DHT22: ERROR");
        statusDisplay.setLine(2,pirReady()?"PIR: LISTO":"PIR: CALENTANDO");
        statusDisplay.setLine(3,"BOOT: CAMBIAR PAG.");
    }
    statusDisplay.render();
}

void ResQMonitorDevice::publishToCloud() {
    const unsigned long now=millis();
    lastPublishMs=now;
    if(!wifiConnection.isConnected()) {
        lastPublishResult=ThingSpeakPublisher::RESULT_NO_CONNECTION;
        Serial.println("[TS] Sin WiFi, no se publica");
        return;
    }
    if(!isReadingValid) {
        lastPublishResult=ThingSpeakPublisher::RESULT_INVALID_SAMPLE;
        Serial.println("[TS] Se omite muestra: temperatura/humedad invalidas");
        return;
    }
    // Field 4 incluye una alerta sonora por gas O un evento PIR.
    // Field 5 distingue especificamente movimiento; la DEMO usa iguales fields.
    const bool motionToPublish=isMotionDetected || pendingMotionCloud;
    const bool alarmToPublish=gasDanger || motionToPublish;
    const char* status=demo.active()?"RESQ ALERTA":"RESQ MONITOREO";
    SafetyStatusRecord record(gasRawLevel,temperatureCelsius,
        wifiConnection.getRssiDbm(),alarmToPublish,motionToPublish,humidityPercent,status);
    lastPublishResult=thingSpeakPublisher.publish(record);
    lastPublishMs=millis(); // evita enviar muy seguido tras peticion HTTP
    Serial.printf("[TS] gas=%d temp=%.1f hum=%.1f mov=%d alerta=%d -> %d\n",
        gasRawLevel,temperatureCelsius,humidityPercent,
        motionToPublish?1:0,alarmToPublish?1:0,lastPublishResult);
    if(lastPublishResult==ThingSpeakPublisher::RESULT_OK) {
        lastCloudOkMs=millis();
        pendingMotionCloud=false;
        demo.acknowledgePublish();
    }
}

void ResQMonitorDevice::printHelp() const {
    Serial.println("[RESQ2] BOOT corto: pagina | doble: menu | largo: iniciar DEMO");
    Serial.println("[RESQ2] DEMO: GAS / TEMPERATURA / HUMEDAD / MOVIMIENTO");
    Serial.println("[RESQ2] SERIAL: SILENCE, UNSILENCE, HELP");
    Serial.println("[RESQ2] No usar como detector certificado de gas/incendio.");
}
