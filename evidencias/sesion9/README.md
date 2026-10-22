# Sesión 9 – Conectividad ESP32 con ThingSpeak (ResQ)

## Integrantes

- TODO
- Pedro TODO
- Gonzalo TODO
- Fernando TODO

## Proyecto y ruta

- Proyecto: **ResQ**, monitor de gas, temperatura y movimiento con alarma
- Ruta lograda: **1** (simulación controlada desde el ESP32 real). TODO: cambiar a 2 o 3 si conectan sensores.

## Datos

Datos **simulados** en el ESP32 real (`SensorSimulator`): gas MQ-2, temperatura DHT22 y movimiento PIR, con ruido alrededor de un valor base. Los actuadores son reales: OLED SSD1306, LED rojo, LED verde y LED de la placa.

Escenario probado: TODO, por ejemplo: "ambiente normal (gas ≈ 600), luego BOOT = fuga de gas (gas ≈ 2800, alarma = 1), luego BOOT otra vez para volver a normal; T=55 para la alerta de temperatura".

## Canal ThingSpeak

- Channel ID público: TODO
- Campos: Field 1 Gas MQ-2 (raw) · Field 2 Temperatura (C) · Field 3 RSSI (dBm) · Field 4 Alarma (0/1) · Field 5 Movimiento (0/1)

## E7 – Análisis cuantitativo (G3_E7_datos.csv)

| Variable | Mínimo | Máximo | Promedio |
|---|---:|---:|---:|
| Gas (raw) | 562 | 2829 | 760.12 |
| Temperatura (°C) | 23.7 | 24.3 | 23.96 |
| RSSI (dBm) | -52 | -22 | -36.43 |

**Envíos exitosos:**
68 recibidos / 68 esperados × 100 = **100 % (estimado)**.

**Interpretación:**

Durante la prueba de conectividad de ResQ se registraron
68 muestras simuladas en ThingSpeak, con intervalos de
20 segundos y una recepción estimada del 100 %.

El valor de gas osciló entre 562 y 2829, registrándose
5 muestras con alarma activa. La temperatura promedio
fue de 23.96 °C y el RSSI promedio de -36.43 dBm,
lo que evidencia una conexión Wi-Fi favorable durante
el periodo analizado.

## Evidencias

E1 entorno · E2 Wi-Fi · E3 canal · E4 prototipo · E5 envíos 200 · E6 gráficos · E7 CSV · E9 repositorio

Video E8: [G##_E8_video.mov](G##_E8_video.mov)
