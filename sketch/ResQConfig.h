#ifndef RESQ_CONFIG_H
#define RESQ_CONFIG_H

// Valores de demostracion: ADC no equivale a ppm. Ajustar con pruebas/calibracion.
// Los parametros se concentran aqui para evitar numeros magicos en el firmware.
namespace ResQConfig {
constexpr int PIN_LED_AMARILLO = 25;
constexpr int GAS_PRECAUCION = 1400;
constexpr int GAS_PELIGRO = 2500;
constexpr int GAS_HISTERESIS = 120;
constexpr float TEMP_PRECAUCION_C = 40.0f;
constexpr float TEMP_PELIGRO_C = 50.0f;
constexpr float TEMP_HISTERESIS_C = 1.5f;
constexpr float HUM_PRECAUCION_PCT = 90.0f;
constexpr float HUM_PELIGRO_PCT = 95.0f;
constexpr float HUM_HISTERESIS_PCT = 3.0f;

constexpr unsigned long PIR_WARMUP_MS = 60000UL;
constexpr unsigned long GAS_WARMUP_MS = 120000UL; // Solo demostrativo; no reemplaza calibracion.
constexpr unsigned long SENSOR_INTERVAL_MS = 2000UL;
constexpr unsigned long OUTPUT_INTERVAL_MS = 20UL;
constexpr unsigned long UI_INTERVAL_MS = 250UL;
constexpr unsigned long PUBLISH_INTERVAL_MS = 20000UL;
constexpr unsigned long WIFI_RETRY_MS = 10000UL;
constexpr unsigned long MOTION_LATCH_MS = 4500UL;
constexpr unsigned long DEMO_MIN_MS = 25000UL;
constexpr unsigned long DEMO_MAX_MS = 90000UL;
constexpr unsigned long DEMO_DONE_HOLD_MS = 2500UL;

constexpr int DEMO_GAS_ADC = 3000;
constexpr float DEMO_TEMP_C = 52.0f;
constexpr float DEMO_HUM_PCT = 97.0f;
constexpr unsigned long RED_BLINK_HALF_MS = 350UL;
constexpr unsigned long YELLOW_BLINK_HALF_MS = 250UL;
}
#endif
