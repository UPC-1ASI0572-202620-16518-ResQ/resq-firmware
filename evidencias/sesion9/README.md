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

## E7 – Análisis cuantitativo (G##_E7_datos.csv)

| Variable | Mínimo | Máximo | Promedio |
| --- | --- | --- | --- |
| Gas (raw) | TODO | TODO | TODO |
| Temperatura (C) | TODO | TODO | TODO |
| RSSI (dBm) | TODO | TODO | TODO |

Envíos exitosos: TODO recibidos ÷ TODO esperados = TODO %

Interpretación: TODO (2 o 3 líneas; por ejemplo, cuándo subió la alarma y si se perdieron envíos).

## Evidencias

E1 entorno · E2 Wi-Fi · E3 canal · E4 prototipo · E5 envíos 200 · E6 gráficos · E7 CSV · E9 repositorio

Video E8: [G##_E8_video.mov](G##_E8_video.mov)
