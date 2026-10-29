# G1 — D5 Gateway y perímetro — Frutos del Chira S.A.C.
**Responsable:** Fabricio Fabian Quispe Barzola  
**Estado:** código y conexiones preparados; en capturas del responsable se observa ejecución de pruebas de almacenamiento con WAN caída y restaurada. Falta guardar enlace permanente de Wokwi y completar pruebas finales (reenvío total, sensores y watchdog).  
**Fuente principal:** caso del docente, "Lab IoT v2 (100 min) – Caso A (G1) Frutos del Chira". La presentación `Arquitectura_IoT_en_el_Borde.pptx` no fue adjuntada.

## Contenido
- `sketch.ino` — demostración completa de lógica perimetral y store-and-forward en microSD, con enlace WAN y ACK **emulados** (no es el firmware de campo).
- `diagram.json` — componentes y conexiones para pegar en Wokwi.
- `libraries.txt` — dependencia RTClib.
- `../../arquitectura/arquitectura-d5.md` — diseño técnico, riesgos, interfaces y almacenamiento.
- `docs/cirkit-paso-a-paso.md` — instalación del D5 en el proyecto Cirkit compartido.
- `docs/mapa-gpio-y-bom.md` — lista de componentes y conexiones de la **propuesta física** (diferente de algunas entradas emuladas en Wokwi).
- `docs/sustentacion-d5.md` — respuesta y guion de 1 minuto con variables pendientes.
- `../../costos/costos-d5.csv` — precios de referencia verificados parcialmente; faltantes sin inventar.
- `../../tablas/ia-z10.md` — alternativas IA / estadística / reglas.
- `../../incidentes/protocolo-v1.md` — protocolo preliminar editable de los 8 capítulos.
- `../../incidentes/protocolo-v2-plantilla.md` — no completar hasta conocer Evento 2.
- `pruebas/Pruebas_Wokwi.md` — bitácora con resultados de capturas reales y pruebas pendientes.
- `pruebas/evidencias/` — cuatro capturas originales recibidas (sin edición).
- `pruebas/PENDIENTES_EJECUTAR.md` — instrucciones para completar evidencia individual.

## Cómo llevar a Wokwi
1. Abrir https://wokwi.com/projects/new/esp32
2. Reemplazar el contenido de `sketch.ino` con `sketch.ino`.
3. Abrir/crear `diagram.json` y pegar el contenido de `diagram.json` de esta carpeta.
4. Añadir `libraries.txt` con `RTClib` si la interfaz lo solicita.
5. Iniciar simulación. Monitor serie: 115200 y final de línea Newline.
6. Probar: `STATUS`, `WAN=0`, `NODE=D1`, `EVENT=CRIT`, esperar lecturas y luego `WAN=1` y `STATUS`.
7. Revisar `events.log` y `acks.log` en el microSD virtual si la interfaz lo permite; comparar los ID.
8. Guardar proyecto y copiar el enlace de Wokwi. NO señalar prueba aprobada hasta ejecutar y capturar resultados.

**Limitaciones:** HC-SR04 sustituye JSN-SR04T, DS1307 sustituye DS3231, flotadores y lluvia son representaciones; mensajes de D1–D4 entran por Monitor Serie, no por RS485; WAN y ACK se simulan; UPS y router 4G no están conectados. Reiniciar una simulación web puede reinicializar su tarjeta SD virtual. El circuito completo debe revisarse y ensayarse antes de conectar hardware.

## Pendiente de coordinación
- Eduardo, Pedro, Fernando y Gonzalo: contratos de mensajes D1–D4, redes locales y consumos de energía.
- D2/Z4: arquitectura de red definitiva tras Evento 1.
- D3/Z6: MB mensuales WAN, costos recurrentes/cobertura en Sullana.
- D4/Z9: alternativa A/B/C elegida, costo total a 3 años y supuestos.
- Docente: detalles de Eventos 1/2.
- Equipo: confirmar nombre del repositorio final y añadir el enlace Wokwi y el video individual.
