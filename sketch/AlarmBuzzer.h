#ifndef ALARM_BUZZER_H
#define ALARM_BUZZER_H

#include <Arduino.h>
// Buzzer activo, con transistor controlador en GPIO 27.
// Sin tone(): los patrones usan pulsos digitales sin bloquear el loop.
enum class BuzzerPattern { SILENT, GAS, MOTION, GAS_AND_MOTION };
class AlarmBuzzer {
public:
    explicit AlarmBuzzer(int pin) : pin(pin) {}
    void begin() { pinMode(pin, OUTPUT); digitalWrite(pin, LOW); }
    void setPattern(BuzzerPattern p) {
        if (pattern != p) { pattern=p; phaseStart=millis(); }
        update();
    }
    void update() {
        unsigned long elapsed=millis()-phaseStart;
        bool on=false;
        if (pattern == BuzzerPattern::GAS) {
            on=gasPulse(elapsed%GAS_CYCLE_MS);
        } else if (pattern == BuzzerPattern::MOTION) {
            on=motionPulse(elapsed%MOTION_CYCLE_MS);
        } else if (pattern == BuzzerPattern::GAS_AND_MOTION) {
            // Dos pitidos ambientales, luego tres de movimiento; sin reiniciar la fase.
            const unsigned long t=elapsed%(GAS_CYCLE_MS+MOTION_CYCLE_MS);
            on=t<GAS_CYCLE_MS ? gasPulse(t) : motionPulse(t-GAS_CYCLE_MS);
        }
        digitalWrite(pin,on?HIGH:LOW);
    }
private:
    static constexpr unsigned long GAS_CYCLE_MS=1600UL, MOTION_CYCLE_MS=1200UL;
    static bool gasPulse(unsigned long t) {
        return t<200UL || (t>=400UL && t<600UL);
    }
    static bool motionPulse(unsigned long t) {
        return t<150UL || (t>=300UL && t<450UL) || (t>=600UL && t<750UL);
    }
    int pin;
    BuzzerPattern pattern=BuzzerPattern::SILENT;
    unsigned long phaseStart=0;
};
#endif
