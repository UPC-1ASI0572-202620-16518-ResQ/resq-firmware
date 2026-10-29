# Plan de prueba D5 / Wokwi — ejecutar, capturar y llenar resultados
Este proyecto **NO ha sido ejecutado en Wokwi por el asistente**. El código y diagrama son archivos para cargar y verificar. El código emula WAN, ACK y los nodos D1–D4 por serial.

## Escenario E0 — Inicio
1. Cargar `sketch.ino`, `diagram.json` y `libraries.txt` (`RTClib`).
2. Iniciar simulación y abrir Monitor Serie a 115200 con Newline.
3. Resultado esperado: `microSD=OK RTC=OK`, `[STORE] ARRANQUE`, `[TX-SIM]`, `[ACK-SIM]`.
4. Comprobar si se muestran mensajes distintos; capturar pantalla.

## Escenario E1 — Caída de WAN
1. Enviar `STATUS`; guardar número de pendientes.
2. Enviar `WAN=0`; se deshabilita solo la WAN **emulada**.
3. Enviar `NODE=D1`, `NODE=D2`, `EVENT=CRIT` y esperar 30 s.
4. Resultado esperado: `[STORE]` aun con WAN=DOWN, sin `[ACK-SIM]` de esas nuevas alarmas; pendientes aumentan.
5. Capturar salida y, si está disponible, contenido de `events.log` de tarjeta microSD virtual.

## Escenario E2 — Recuperación
1. Enviar `WAN=1`.
2. Esperar varios intervalos de 2.5 s: primero P0 (inundación), luego P1 (nodos), después P2 (resúmenes).
3. Enviar `STATUS` para verificar que disminuye `pendientes`.
4. Revisar `acks.log`: IDs ya confirmados por el **simulador de servidor**, no una nube real.
5. Capturar pantalla.

## Escenario E3 — Riesgo perimetral
1. Mover un potenciómetro de lluvia a bajo valor: resultado esperado `ALERTA` si el nivel es menor que `RAIN_THRESHOLD`.
2. Pasar interruptor float1 o float2 a contacto GND: resultado esperado `CRITICO` y prioridad P0.
3. Variar distancia de un HC-SR04 bajo el umbral de prueba (>25 cm y <40 cm): resultado esperado `CRITICO`.
4. Restablecer valores y comprobar retorno a NORMAL; conservar capturas.

## Escenario E4 — Reinicio / persistencia
1. Reiniciar simulador web y observar si se recrea microSD. **No usar esta acción como prueba de persistencia entre sesiones**: Wokwi web genera FAT16 y no garantiza que conserve escrituras entre sesiones.
2. La persistencia después de pérdida de energía en hardware físico debe probarse con tarjeta real, energía estable y pruebas de corte; aún está pendiente.

## Matriz de resultados (rellenar SOLO tras ejecutar)
| Escenario | Resultado esperado | Resultado observado | Captura / enlace | Estado |
|---|---|---|---|---|
| E0 | microSD y RTC inicializados | PENDIENTE | PENDIENTE | NO EJECUTADO |
| E1 | Crecimiento de pendientes al caer WAN | PENDIENTE | PENDIENTE | NO EJECUTADO |
| E2 | Reenvío prioritario + ACK simulado | PENDIENTE | PENDIENTE | NO EJECUTADO |
| E3 | Alerta de lluvia, CRÍTICO por inundación | PENDIENTE | PENDIENTE | NO EJECUTADO |
| E4 | Delimitar comportamiento virtual, no afirmar persistencia de reinicio | PENDIENTE | PENDIENTE | NO EJECUTADO |

## Referencias
- microSD Wokwi: https://docs.wokwi.com/parts/wokwi-microsd-card
- Ultrasonido sustituto: https://docs.wokwi.com/parts/wokwi-hc-sr04
- RTC sustituto: https://docs.wokwi.com/parts/wokwi-ds1307
