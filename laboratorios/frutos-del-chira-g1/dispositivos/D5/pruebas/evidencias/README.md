# Evidencias D5 capturadas en Wokwi

Estas imágenes provienen de capturas de pantalla enviadas por Fabricio durante su ejecución del **código inicial** el 29/10/2026. No han sido retocadas ni creadas para aparentar una ejecución.

| Archivo | Qué se ve | Qué NO demuestra |
|---|---|---|
| 01-arranque.png | El ESP32 y monitor inicial; RTC/SD y envíos simulados | WAN real, hardware físico |
| 02-wan-caida.png | Línea de caída de internet emulado | Persistencia ante apagón |
| 03-cola-pendiente.png | `WAN=DOWN` y `pendientes=26` | Reenvío al volver internet |
| 04-acumulacion.png | `WAN=0 enCola=37` y nuevos `STORE` | Que la cola se vacíe |
| 05-wan-restaurada.png | `WAN` emulada restaurada | ACK posterior a recuperación (no visible) |

Las capturas 04 y 05 son la **misma imagen** porque muestra ambos hechos; se mantiene una sola evidencia conceptual, sin presentarla como dos pruebas independientes.

Falta adjuntar: captura con ACK posterior al `WAN=1`, captura de ultrasonido/float en `CRITICO`, captura de lluvia estabilizada, captura de `FALLA_SENSOR`, captura del watchdog actualizado, captura de D5 en Cirkit y enlace guardado Wokwi.
