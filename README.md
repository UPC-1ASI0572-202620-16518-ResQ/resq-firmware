# resq-firmware
ESP32 firmware for ResQ IoT gas monitoring and alert system

## ¿Qué es ResQ?

ResQ es un monitor de seguridad para ambientes cerrados. Mide **gas/humo** (MQ-2), **temperatura** (DHT22) y **movimiento** (PIR) con un ESP32. Avisa con LED verde/rojo, buzzer y una pantalla OLED, y envía los datos a la nube.

## Integrantes

| Integrante |
| --- |
| Fernando |
| Gonzalo |
| Pedro |
| Fabricio |
| Eduardo |

## Lab 2: conectividad con ThingSpeak (Ruta 1)

El ESP32 real se conecta por Wi-Fi y envía cada 20 s a ThingSpeak datos **simulados** de los sensores, que todavía no están conectados. El OLED y los LEDs sí son reales. La simulación se controla desde el monitor serie (`G=valor`, `T=valor`, `M=1/0`) y con el botón **BOOT**, que simula una fuga de gas.

| Campo | Dato |
| --- | --- |
| Field 1 | Gas MQ-2 (raw) |
| Field 2 | Temperatura (°C) |
| Field 3 | RSSI (dBm) |
| Field 4 | Alarma (0/1) |
| Field 5 | Movimiento (0/1) |

**Canal público de ThingSpeak:** https://thingspeak.mathworks.com/channels/CHANNEL_ID

## Cómo correrlo

1. Instalar en Arduino IDE las librerías **ThingSpeak** (MathWorks), **DHT sensor library** y **Adafruit SSD1306** (Adafruit).
2. Abrir `sketch/sketch.ino`.
3. Copiar `sketch/secrets.example.h` como `sketch/secrets.h` y llenar el Wi-Fi (2.4 GHz), el Channel ID y la Write API Key. `secrets.h` está en `.gitignore` y no se sube.
4. Placa **DOIT ESP32 DEVKIT V1**, subir el programa y abrir el monitor serie a **115200** con **New Line**.

## Pines

| Componente | Pin ESP32 |
| --- | --- |
| OLED SDA / SCL (0x3C) | 21 / 19 |
| LED rojo / verde | 22 / 23 |
| MQ-2 AO / DHT22 / PIR | 34 / 4 / 33 |
| Buzzer | 27 |

## Evidencias

Las evidencias de cada sesión están en [`evidencias/`](evidencias/).
