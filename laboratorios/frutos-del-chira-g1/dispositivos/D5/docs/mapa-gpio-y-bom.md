# Mapa GPIO y BOM — Propuesta de hardware D5
Responsable: Fabricio Fabian Quispe Barzola.

## 1. Mapa GPIO de la PROPUESTA FÍSICA (ESP32-WROOM-32 / DevKit V1)
| Componente | Pin de sensor | ESP32 o bus | Protección / nota |
|---|---|---|---|
| JSN 1 | TRIG/ECHO | 13 / 32 | ECHO por divisor 10k/18k desde 5V a ~3.21V |
| JSN 2 | TRIG/ECHO | 14 / 33 | ECHO por divisor |
| JSN 3 | TRIG/ECHO | 25 / 34 | GPIO34 solo entrada, sin pull-up interno |
| JSN 4 | TRIG/ECHO | 26 / 35 | GPIO35 solo entrada, sin pull-up interno |
| FC-37 R1 | AO | 36 (ADC1) | Módulo a 3.3V si versión compatible; nunca AO >3.3V |
| FC-37 R2 | AO | 39 (ADC1) | ADC1 permite lectura con Wi-Fi activo |
| Flotador F1 | contacto seco | MCP23017 GPA0 | Pull-up 3.3V; otro cable a GND |
| Flotador F2 | contacto seco | MCP23017 GPA1 | Pull-up 3.3V; otro cable a GND |
| DS3231 RTC | SDA/SCL | 21 / 22 | I2C 3.3V; verificar placa RTC y pila |
| MCP23017 | SDA/SCL | 21 / 22 | I2C 0x20; A0/A1/A2 de dirección a GND |
| microSD | SCK/MISO/MOSI/CS | 18 / 19 / 23 / 5 | Módulo de 3.3V o nivel compatible; GPIO5 strapping |
| RS485 hacia D1–D4 | TX/RX/DE-RE | 17 / 16 / 27 | UART2; transceptor 3.3V aislado preferente |
| Router LTE | Wi-Fi AP | Sin GPIO | La conexión de D5 al router es Wi-Fi; WAN principal por Ethernet y respaldo LTE |

**Atención:** no conectar ECHO de 5V sin adaptación. La proporción 18/(10+18) produce ~3.21 V ante 5 V nominal, antes de tolerancias; verificar al medir. Transductor JSN resistente al agua, **no** su placa electrónica. Evitar GPIO 0, 2, 12, 15 y periféricos de flash GPIO6–11. La simulación Wokwi usa GPIO16/17 para flotadores, pues allí no representa RS485. En el circuito físico se usa MCP23017 para liberar esos pines.

## 2. Lista de materiales y fuentes de precios
`O` = obligatorio del caso; `P` = adicional propuesto. El consumo máximo de varios elementos depende de variante: solicitar hoja de datos exacta antes de calcular UPS.

| Tipo | Componente / modelo propuesto | Cant. | Interfaz/tensión | Consumo | GPIO/bus | Precio ud. (S/) | Subtotal (S/) | Fuente/observación |
|---|---|---:|---|---|---|---:|---:|---|
| P | ESP32 DevKit V1 ESP-WROOM-32 | 1 | 5V USB, I/O 3.3V | variable, Wi-Fi con picos | múltiples | 36.00 | 36.00 | INTECX tienda |
| O | JSN-SR04T (impermeable transductor) | 4 | 5V TRIG/ECHO | 8–30mA según variante | TRIG+ECHO | 40.00 | 160.00 | MTLab; verificar versión |
| O | FC-37/YL-83 lluvia | 2 | AO 3.3V verificado | ~15mA según variante | ADC1 | 10.00 | 20.00 | Naylamp; puede estar agotado |
| O | Flotador vertical contacto seco | 2 | contacto + pull-up | pasivo | MCP23017 | 12.00 | 24.00 | Naylamp |
| O | RTC DS3231 | 1 | I2C 3.3V posible | verificar módulo | I2C | 24.00 | 24.00 | Electromanía; batería incluida según opción |
| O | Módulo microSD SPI | 1 | 3.3/5V según placa | variable | SPI | 14.00 | 14.00 | Edubotics |
| O | Tarjeta microSD 8GB/16GB | 1 | microSD | variable | SPI | pendiente | pendiente | Cotizar resistencia/calidad |
| P | MCP23017 expansor de GPIO | 1 | I2C 3.3V | verificar datasheet | I2C | pendiente | pendiente | Evita pines strap para flotadores |
| P | RS485 transceptor aislado 3.3V | 1 | UART/RS485 | verificar referencia | 16/17/27 | pendiente | pendiente | Cotizar módulo exacto de 3.3V |
| P | Divisores ECHO, resistores 10k +18k | 4 pares | pasivo | despreciable | ECHO | pendiente | pendiente | Protección de lógica 3.3V |
| P | Router industrial 4G LTE RUT200 | 1 | Ethernet WAN/LTE/Wi-Fi | según ficha/modelo | WAN externa | 490.00 | 490.00 | Mediatech; interfaz dual WAN a validar con configuración |
| P | Conversor/fuente 5V 3.3V estabilizada | 1 | DC | dimensionar | alimentación | pendiente | pendiente | No estimar autonomía sin cargas |
| P | UPS/batería/conmutación | 1 | DC | dimensionar | alimentación | pendiente | pendiente | No cubre 72h por defecto |
| P | SPD, gabinetes IP65, cable y conectores | 1 lote | protección | no aplica | física | pendiente | pendiente | Cotizar según instalación |

**Subtotal de filas con precio publicado**: S/ 768.00. NO equivale al precio total del D5: excluye muchos componentes, instalación, energía, mantenimiento y envío. El RUT200 es una alternativa industrial de referencia y puede ser demasiado costoso para la alternativa A. Además el costo recurrente del plan móvil es separado del equipo.

Fuentes consultadas (29/10/2026):
- ESP32: https://store.intecx.com.pe/product/modulo-esp32-devkit-usb-tipo-c/
- JSN: https://mtlab.pe/producto/jsn-sr04t-sensor-ultrasonico-5-v-impermeable-25-450-cm/
- Lluvia: https://naylampmechatronics.com/sensores/216-sensor-de-lluvia-y-nieve-fc-37.html
- Flotador: https://naylampmechatronics.com/sensores-liquido/120-sensor-de-nivel-de-liquido-vertical.html
- RTC: https://www.electromania.pe/producto/modulo-rtc-ds3231/
- SD: https://eduboticsperu.com/tienda/modulo-micro-sd/
- Router: https://mediatechperu.com/producto/enrutador-iot-inalambrico-teltonika-rut200-industrial-lte-wifi-4g-negro
- Restricciones de GPIO: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html
