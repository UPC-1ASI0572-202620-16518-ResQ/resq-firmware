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

## Firmware actual: ruta 3, sensores reales y alarma local

MQ en GPIO 34 y DHT22/AM2302 en GPIO 4 son reales. El PIR permanece
simulado (`M=1` / `M=0`). `G=` y `T=` no sustituyen sensores reales.
La humedad se monitoriza, sin umbrales de alarma.

| Campo ThingSpeak | Dato |
| --- | --- |
| Field 1 | Gas MQ (ADC, 0–4095; no ppm) |
| Field 2 | Temperatura (°C) |
| Field 3 | RSSI Wi-Fi (dBm) |
| Field 4 | Alarma de peligro (0/1, sigue en 1 al silenciar) |
| Field 5 | Movimiento simulado (0/1) |
| Field 6 | Humedad relativa (%) |

Habilitar **Field 6** en la configuración del canal ThingSpeak. Se conserva
un único envío cada 20 s; si falla el DHT, se omite la muestra completa.
El publicador también rechaza NaN/infinito antes de preparar la petición.
Wi-Fi se reconecta sin esperar en cada ciclo de publicación cuando está desconectado.
Nunca se imprimen contraseñas ni claves.

### Muestreo, seguridad y OLED

El DHT se consulta como máximo cada 2000 ms, incluso al usar BOOT/comandos.
Después de `begin()` espera 2 s antes del primer intento. Una lectura fallida
invalida temperatura y humedad inmediatamente; no reutiliza el último dato correcto.
Al recuperarse el sensor se reanudan las mediciones y los envíos en su intervalo normal.

Se conservan los umbrales: gas 1400/2500 ADC y temperatura 40/50 °C
para PRECAUCIÓN/PELIGRO. PELIGRO tiene prioridad, incluso durante los 20 s
de calentamiento del MQ o ante un error DHT. Sin peligro conocido, un fallo
DHT muestra ERROR SENSOR DHT: verde apagado y rojo intermitente, sin convertir
el fallo en alarma de temperatura. NORMAL enciende verde; PRECAUCIÓN y
calentamiento hacen parpadear rojo; PELIGRO enciende rojo y activa la salida
del buzzer (salvo `SILENCE`). La humedad no modifica estas reglas.

OLED 128 × 64: encabezado RESQ y cuatro líneas de hasta 21 caracteres:
gas/movimiento, temperatura/humedad, seguridad, Wi-Fi/último envío.
El grado usa la fuente CP437 y los estados usan texto ASCII para evitar
acentos cortados. Si coinciden peligro por gas y fallo DHT, se ven ambos.

### Comprobación física pendiente

1. Conectar el DHT de tres pines: `+` a 3.3 V, `OUT` a GPIO 4, `−` a GND.
2. Verificar en Serie (115200) temperatura y HR, y las dos lecturas en OLED.
3. Desconectar OUT: tras el siguiente intento (hasta 2 s, sin bloqueos de red),
   comprobar ERROR DHT, verde apagado, rojo intermitente y ausencia de nuevos
   registros ThingSpeak; reconectar y comprobar recuperación.
4. Verificar Field 1–6 y espaciado de al menos 20 s entre intentos de envío.
5. Verificar LEDs y la salida GPIO 27 bajo peligro y `SILENCE`/`UNSILENCE`.
   El funcionamiento físico del buzzer no está confirmado. No provocar una
   fuga real para probar alarmas; usar la ruta simulada en una prueba controlada.

Limitaciones heredadas: conexión Wi-Fi inicial bloqueante hasta 15 s y envío
HTTP síncrono de ThingSpeak; durante esos bloqueos el bucle no toma muestras.
MQ tiene umbrales de demostración y requiere calibración/estabilización física.

## Cómo correrlo

1. Instalar en Arduino IDE las librerías **ThingSpeak** (MathWorks), **DHT sensor library** (1.4.6), **Adafruit Unified Sensor**, **Adafruit SSD1306**, **Adafruit GFX Library** y **Adafruit BusIO**. ThingSpeak verificado con 2.1.1.
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
