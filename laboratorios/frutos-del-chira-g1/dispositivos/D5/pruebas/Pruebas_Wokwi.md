# Pruebas D5 en Wokwi — Bitácora honesta

**Responsable:** Fabricio Fabian Quispe Barzola (D5).  
**Modalidad:** simulación, sin radio 4G ni RS485 físicos; nodos D1–D4 y confirmaciones de servidor emulados.

## Evidencia observada en capturas del 29-10-2026 (versión inicial)

| Caso | Registro observado | Resultado | Evidencia |
|---|---|---|---|
| Inicio | `microSD=OK RTC=OK`, `ARRANQUE`, `STORE` y `TX-SIM/ACK-SIM` | Verificado visualmente | `evidencias/01-arranque.png` |
| Desconexión | Mensaje `WAN ... CAIDOS`; `STATUS ... WAN=DOWN ... pendientes=26` | Verificado visualmente | `evidencias/02-wan-caida.png`, `03-cola-pendiente.png` |
| Retención en desconexión | `WAN=0 enCola=37`, nuevos eventos `STORE` con `WAN=DOWN` | Verificado visualmente | `evidencias/04-acumulacion.png` |
| Reconexión | Mensaje `[WAN] Enlace emulado RESTAURADO` | Verificado visualmente | `evidencias/04-acumulacion.png` |
| Retransmisión posterior a recuperación | **No visible** `[ACK-SIM]` posterior a restauración, ni cola vacía | Pendiente | Nueva captura requerida |
| Activación física virtual de inundación | No se ve sensor a 30 cm ni flotador activo en los registros suministrados | Pendiente | Nueva captura requerida |
| Watchdog e histéresis actualizados | Código corregido y aún **no ejecutado** en Wokwi | Pendiente | Nueva captura requerida |
| Reinicio y persistencia | No demostrado sobre microSD virtual | Pendiente | Nueva prueba requerida |

**Alcance:** las imágenes provienen de la ejecución realizada por el estudiante y enviada al chat. No son capturas creadas o manipuladas por el asistente. Las observaciones se refieren a la versión anterior a la corrección de lluvia/watchdog.

## Cómo finalizar la prueba con el código actualizado

1. Copiar el nuevo `sketch.ino` al proyecto Wokwi; iniciar Play. Confirmar `microSD=OK`, `RTC=OK` y `[WDT] ACTIVO`.
2. Elegir Newline en el Monitor Serie; enviar **individualmente** `WAN=0`, `NODE=D1`, `NODE=D2`, `EVENT=CRIT`, `STATUS`.
3. Confirmar `WAN=DOWN` con mensajes pendientes y sin ACK simulados nuevos durante la desconexión. Capturar.
4. Enviar `WAN=1`, esperar a los mensajes `[TX-SIM]` y `[ACK-SIM]` y comprobar `STATUS` con pendientes decrecientes. Capturar. **No declarar cola vacía hasta verla**.
5. Ajustar un HC-SR04 virtual a 30 cm (>=25 cm para evitar lectura inválida); debe observarse `CRITICO`. Volver a >=50 cm para despejar la alarma; capturar.
6. Activar un flotador virtual (contacto a GND) y observar `CRITICO`; desactivarlo; capturar.
7. Mover un potenciómetro por debajo de 1500, esperar cuatro evaluaciones; debe aparecer `[LLUVIA] Confirmada` y `ALERTA`. Para despejar, ambos deben superar 2000 durante cuatro evaluaciones. Capturar.
8. Provocar lectura ultrasónica inválida de una sonda tres veces consecutivas y observar `FALLA_SENSOR` cuando no haya `CRITICO`.
9. Guardar el proyecto con Save y copiar URL `/projects/<id>` a `../../../../entregables/enlaces-wokwi.md`.

**Limitaciones:** SD virtual no demuestra persistencia física ante apagón; no hay router LTE ni intercambio real con D1–D4. El watchdog debe comprobarse ejecutando el firmware. Los umbrales son **solo didácticos**. Si SD falla, la alarma local debe seguir informando el problema pero no se garantiza conservación de datos.
