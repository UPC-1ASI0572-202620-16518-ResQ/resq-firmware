# ResQ 2.0 — firmware ESP32

Firmware del prototipo físico ResQ: monitoreo ambiental, detección de movimiento, alertas locales, OLED interactiva y publicación a ThingSpeak.

 > **Estado:** versión integrada para pruebas académicas; validar en hardware después de cualquier cambio de firmware.
>
> **Seguridad:** prototipo académico. No reemplaza un detector certificado de gas, incendios o intrusión. Las lecturas MQ-2 en ADC **no son ppm**, y los umbrales requieren calibración.

## Hardware y conexiones

| Elemento | Conexión ESP32 |
|---|---|
| OLED 128×64 SSD1306 | SDA GPIO21, SCL GPIO19, VCC 3V3, GND |
| Sensor MQ-2 | VCC 5V (VIN con USB), GND; AO a GPIO34 **mediante divisor de tensión** |
| Sensor DHT22 (módulo 3 pines) | + a 3V3, OUT a GPIO4, − a GND |
| Sensor HC-SR501 PIR | VCC a 5V, OUT a GPIO33, GND |
| LED verde + resistencia 220 Ω | GPIO23 |
| LED rojo + resistencia 220 Ω | GPIO22 |
| LED amarillo + resistencia 220 Ω | GPIO25 |
| Buzzer activo + etapa transistor | GPIO27, con resistencia de base y transistor adecuados |
| Botón integrado BOOT | GPIO0, sin cable extra |

**Divisor MQ-2 del montaje:** cuatro resistencias de 10 kΩ en serie entre AO y GND; el punto medio (después de dos resistencias) va a GPIO34. Con AO=5 V, ese esquema ofrece aproximadamente 2.5 V en GPIO34. Comprobar todas las filas de la protoboard antes de alimentar. No usar el AO de 5V directamente en el ESP32.

El MQ-2 necesita calentamiento y calibración, y el calefactor metálico se calienta al funcionar; evitar tocarlo. PIR y MQ-2 pueden sobrecargar una alimentación USB insuficiente. Los rieles 3.3V y 5V deben permanecer separados y los GND deben ser comunes.

## Estados e indicadores

Los estados ambientales y el movimiento se controlan **independientemente**:

| Situación | Verde | Rojo | Amarillo | Buzzer |
|---|---|---|---|---|
| Ambiente normal | Fijo | Apagado | Según movimiento | Silencio si no hay movimiento |
| Gas peligro | Apagado | Fijo | Según movimiento | Dos pitidos cortos por ciclo |
| Gas precaución | Apagado | Fijo | Según movimiento | Dos pitidos cortos por ciclo |
| Temperatura/humedad alta | Apagado | Fijo | Según movimiento | Dos pitidos cortos por ciclo |
| Movimiento PIR | Según ambiente | Según ambiente | Intermitente | Tres pitidos cortos por ciclo |
| Error DHT / calentamiento MQ-2 | Apagado | Intermitente | Según movimiento | Silencio, salvo movimiento |

**Prioridad sonora:** cualquier alerta ambiental (gas, temperatura o humedad) > movimiento > silencio. Todos los eventos ambientales usan dos pitidos por ciclo; movimiento usa tres. Un calentamiento normal o una falla DHT muestran aviso visual, sin considerarse por sí solos una alerta ambiental. Umbrales e intervalos se definen en `sketch/ResQConfig.h`; no están distribuidos como valores mágicos en el controlador.

El PIR real se estabiliza durante 60 segundos. El MQ-2 tiene una espera inicial demostrativa de 120 segundos, insuficiente como validación de estabilidad/calibración comercial. Las mediciones del DHT22 se realizan cada 2 segundos. Los patrones de LED y buzzer no usan `delay()`.

## OLED: tres páginas fijas

Una pulsación breve de **BOOT** cambia entre:
1. **RESQ / Principal:** gas, temperatura, humedad, PIR y alerta vigente.
2. **RED / NUBE:** Wi-Fi, RSSI, estado ThingSpeak y segundos desde último envío confirmado.
3. **DIAGNÓSTICO:** estado DHT22, PIR y MQ-2.

No hay rotación automática. Una alerta muestra un aviso destacado sobre la página seleccionada; al terminar se recupera la página anterior. Los avisos de simulación llevan etiqueta **DEMO** solo en la OLED. Durante un escenario activo se indica su nombre.

**Cuidado:** no mantener BOOT presionado mientras se reinicia el ESP32; GPIO0 participa en el modo de descarga del chip.

## Modo de demostración con BOOT

| Acción | Resultado |
|---|---|
| Una pulsación corta | Página siguiente; dentro del menú, escenario siguiente |
| Doble pulsación | Abre/cierra el menú DEMO |
| Pulsación larga (aprox. 1.6 s) en menú | Ejecuta el escenario seleccionado |
| Pulsación larga en ejecución | Cancela la demostración |

Escenarios: **GAS**, **TEMPERATURA**, **HUMEDAD**, **MOVIMIENTO**. Se inyectan valores de prueba desde `ResQConfig.h` al mismo motor de evaluación de sensores reales:
- GAS: 3000 ADC.
- TEMPERATURA: 52.0 °C.
- HUMEDAD: 97 %.
- MOVIMIENTO: activo.

La demostración permanece como mínimo 25 s una vez publicado correctamente o hasta un máximo de 90 s si no se puede publicar; puede cancelarse manualmente. No se necesita liberar gas, aumentar calor real ni humedecer el DHT22.

**Política académica acordada:** los escenarios se publican por los **mismos fields** y umbrales que las mediciones normales, sin un campo extra que los separe en ThingSpeak. **Son datos sintéticos:** la etiqueta DEMO se conserva localmente en OLED y el procedimiento se documenta aquí para que las evidencias de evaluación sean transparentes. Durante una demo, la alerta real de gas no se oculta.

## ThingSpeak

| Field | Contenido |
|---|---|
| Field 1 | Gas MQ-2 ADC (no ppm) |
| Field 2 | Temperatura °C |
| Field 3 | Wi-Fi RSSI dBm |
| Field 4 | Alerta general del sistema: gas, temperatura, humedad o movimiento (0/1) |
| Field 5 | Movimiento (0/1), incluida una detección retenida hasta publicar |
| Field 6 | Humedad relativa % |

Un intento de escritura cada 20 segundos (respetando el intervalo mínimo del canal). El PIR conserva el evento pendiente hasta un envío exitoso para que un pulso corto no desaparezca antes del registro. Si el DHT real no produce temperatura/humedad válidas, se omite la muestra completa, porque ThingSpeak recibe un conjunto coherente.

La conexión Wi-Fi se inicia y reintenta **sin esperar en el bucle local**. La petición HTTP de la biblioteca ThingSpeak **continúa siendo síncrona** y podría introducir una pausa temporal: es una limitación por validar en hardware.

Para probar escenarios sin pulsar BOOT, el Monitor Serie (115200, fin de línea New Line) acepta los comandos `DEMO=GAS`, `DEMO=TEMP`, `DEMO=HUM`, `DEMO=MOV` y `DEMO=OFF`. Las demostraciones también funcionan mediante el botón físico.

La integración con el **frontend** no está en este repositorio: será la fase posterior, después de probar y fusionar este firmware y de verificar ThingSpeak. El frontend necesitará un flujo de ingestión compatible (por ejemplo gateway/MQTT) para reflejar la misma telemetría.

## Cómo compilar y probar

1. Descargar ZIP de **esta rama**, no `main`, y extraer la carpeta completa.
2. Abrir `sketch/sketch.ino` en Arduino IDE, manteniendo los archivos `.h` y `.cpp` contiguos.
3. Instalar bibliotecas: **ThingSpeak** de MathWorks, **DHT sensor library**, **Adafruit Unified Sensor**, **Adafruit SSD1306**, **Adafruit GFX** y **Adafruit BusIO**. La placa es **DOIT ESP32 DEVKIT V1**, Monitor Serie a **115200**.
4. Copiar `sketch/secrets.example.h` a `sketch/secrets.h` y completar SSID Wi-Fi 2.4 GHz, contraseña, Channel ID y Write API Key. Nunca subir `secrets.h` a GitHub.
5. Verificar el cableado y el divisor MQ-2 **con USB desconectado** antes de alimentar.
6. Compilar, subir y comprobar OLED/LED/buzzer/sensores. Esperar estabilización.
7. Verificar navegación BOOT, las cuatro demos y los mensajes `[TS]` con código **200**. Revisar los seis campos del canal y anotar la hora/escenario de cada prueba.
8. Probar desconexión Wi-Fi y restauración; confirmar que LED y buzzer siguen respondiendo localmente.

### Comprobaciones antes del merge

- [ ] Compila en Arduino IDE sin errores.
- [ ] OLED muestra las tres páginas y recupera la última tras alerta.
- [ ] LED amarillo y tres pitidos con PIR real; verde permanece según estado ambiental.
- [ ] Gas (solo DEMO): LED rojo fijo y dos pitidos.
- [ ] Temperatura y humedad (solo DEMO): LED rojo fijo y dos pitidos (igual que gas).
- [ ] Doble BOOT abre el menú, largo ejecuta demo; ninguna pulsación reinicia accidentalmente la placa.
- [ ] Se publican seis fields de ThingSpeak y se ve código HTTP 200.
- [ ] Los eventos cortos PIR aparecen en Field 5.
- [ ] Recuperación después de pérdida Wi-Fi sin perder alarmas locales.
- [ ] No se publica `secrets.h`, ni datos privados.

## Archivos relevantes

- `ResQConfig.h`: parámetros y umbrales editables.
- `DemoController.h`: escenarios y ciclo de prueba.
- `PushButton.h`: navegación con pulsaciones corta, doble y larga.
- `AlarmBuzzer.h`: patrones no bloqueantes.
- `OledStatusDisplay.h`: render y títulos de cada página.
- `ResQMonitorDevice.h/.cpp`: integración de sensores, estado, salidas y nube.
- `ThingSpeakPublisher.h`: mapeo de los seis fields.
