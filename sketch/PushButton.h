#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H

#include <Arduino.h>

// BOOT / GPIO0: pulsacion corta = pagina; doble = menu; larga = ejecutar demo.
// Mantener BOOT presionado DURANTE el reinicio puede entrar en modo descarga.
enum class ButtonEvent { NONE, SHORT, DOUBLE, LONG };
class PushButton {
public:
    explicit PushButton(int pin) : pin(pin) {}
    void begin() {
        pinMode(pin, INPUT_PULLUP);
        stable = digitalRead(pin);
        lastRaw = stable;
    }
    ButtonEvent poll() {
        const unsigned long now = millis();
        const int raw = digitalRead(pin);
        if (raw != lastRaw) { lastRaw = raw; changedAt = now; }
        if (raw != stable && now-changedAt >= DEBOUNCE_MS) {
            stable = raw;
            if (stable == LOW) { pressedAt = now; longReported = false; }
            else if (!longReported) {
                if (waitingSecond && now-firstReleaseAt <= DOUBLE_MS) {
                    waitingSecond = false;
                    return ButtonEvent::DOUBLE;
                }
                waitingSecond = true;
                firstReleaseAt = now;
            }
        }
        if (stable == LOW && !longReported && now-pressedAt >= LONG_MS) {
            longReported = true;
            waitingSecond = false;
            return ButtonEvent::LONG;
        }
        if (stable == HIGH && waitingSecond && now-firstReleaseAt > DOUBLE_MS) {
            waitingSecond = false;
            return ButtonEvent::SHORT;
        }
        return ButtonEvent::NONE;
    }
private:
    static constexpr unsigned long DEBOUNCE_MS=35UL;
    static constexpr unsigned long DOUBLE_MS=420UL;
    static constexpr unsigned long LONG_MS=1600UL;
    int pin, stable=HIGH, lastRaw=HIGH;
    unsigned long changedAt=0, pressedAt=0, firstReleaseAt=0;
    bool longReported=false, waitingSecond=false;
};
#endif
