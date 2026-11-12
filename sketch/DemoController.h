#ifndef DEMO_CONTROLLER_H
#define DEMO_CONTROLLER_H

#include <Arduino.h>
#include "ResQConfig.h"

// Los escenarios se procesan como mediciones habituales, no como una ruta
// paralela. ThingSpeak recibe los mismos seis fields sin indicadores extra.
enum class DemoScenario : uint8_t { GAS=0, TEMPERATURE=1, HUMIDITY=2, MOTION=3 };
class DemoController {
public:
    bool menuOpen() const { return menu; }
    bool active() const { return running; }
    DemoScenario selected() const { return choice; }
    const char* selectedName() const { return scenarioName(choice); }
    void toggleMenu() {
        if (running) { stop(); return; }
        menu = !menu;
    }
    void next() {
        if (!menu || running) return;
        choice = static_cast<DemoScenario>((static_cast<int>(choice)+1)%4);
    }
    void startScenario(DemoScenario s, unsigned long now) { if(running) stop(); choice=s; menu=true; start(now); }
    void start(unsigned long now) {
        if (!menu || running) return;
        running = true;
        menu = false;
        delivered = false;
        startedAt = now;
    }
    void stop() { running = false; menu = false; delivered = false; }
    void acknowledgePublish() { if (running) delivered = true; }
    void update(unsigned long now) {
        if (running && ((delivered && now-startedAt >= ResQConfig::DEMO_MIN_MS)
                     || now-startedAt >= ResQConfig::DEMO_MAX_MS)) {
            running = false;
        }
    }
    static const char* scenarioName(DemoScenario s) {
        switch (s) {
            case DemoScenario::GAS: return "GAS";
            case DemoScenario::TEMPERATURE: return "TEMPERATURA";
            case DemoScenario::HUMIDITY: return "HUMEDAD";
            case DemoScenario::MOTION: return "MOVIMIENTO";
        }
        return "DESCONOCIDO";
    }
private:
    bool menu = false;
    bool running = false;
    bool delivered = false;
    unsigned long startedAt = 0;
    DemoScenario choice = DemoScenario::GAS;
};
#endif
