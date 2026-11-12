#ifndef ALARM_BUZZER_H
#define ALARM_BUZZER_H

#include <Arduino.h>
// Buzzer activo, con transistor controlador en GPIO 27.
// Sin tone(): los patrones usan pulsos digitales sin bloquear el loop.
enum class BuzzerPattern { SILENT, GAS, MOTION };
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
            unsigned long t=elapsed%1600UL;
            on=(t<200UL)||(t>=400UL && t<600UL);
        } else if (pattern == BuzzerPattern::MOTION) {
            unsigned long t=elapsed%1200UL;
            on=(t<150UL)||(t>=300UL && t<450UL)||(t>=600UL && t<750UL);
        }
        digitalWrite(pin,on?HIGH:LOW);
    }
private:
    int pin;
    BuzzerPattern pattern=BuzzerPattern::SILENT;
    unsigned long phaseStart=0;
};
#endif
