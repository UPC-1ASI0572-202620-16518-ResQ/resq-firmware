# Z10 — IA, reglas y estadística
**Consolida:** Fabricio Fabian Quispe Barzola (D5). Las decisiones deben validarse con D1–D4.

| Situación | Técnica | Dónde | Datos / justificación | Falso positivo | Falso negativo |
|---|---|---|---|---|---|
| Degradación compresor | Reglas + media móvil, dispersión, tendencia y umbral adaptativo simple (NO IA compleja) | D4 | Potencia/corriente PZEM, vibración MPU6050 y estado de compresores. Sin histórico etiquetado no se puede validar un modelo ML | Inspección/parada innecesaria y costo operativo | Pérdida de refrigeración, posible lote rechazado USD 25,000 (caso) |
| Excursión de temperatura | Reglas auditable + validación de 2/3 cuando hay 3 lecturas comparables | D1/D2/D3 y D5 | Temperatura por cámara y tiempo de excursión; conservar lectura, sensor, hora y decisión | Alarmas innecesarias y aperturas que aumentan carga térmica | Producto fuera de especificación sin alerta; rechazos |
| Inundación | Umbrales/histéresis + confirmación multisensor, NO IA | D5 | 4 distancias del agua, 2 lluvias, 2 flotadores; instalar referencia de alturas y validar polaridad | Falsa evacuación/parada | Inundación eléctrica/personas en peligro; daño físico |

**Criterio:** predominan reglas por explicabilidad y pocos datos. IA podría investigarse después de recopilar datos históricos, patrones de falla etiquetados y métricas de sensibilidad/falsos negativos. No atribuir esto al profesor: el caso PERMITE no usar IA con justificación.

## Fuentes técnicas
- Wokwi hardware: https://docs.wokwi.com/getting-started/supported-hardware
- microSD en Wokwi: https://docs.wokwi.com/parts/wokwi-microsd-card
- ESP32 GPIO y ADC: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html
- Caso y premisas de negocio: documento del docente, Frutos del Chira (ficticia).
