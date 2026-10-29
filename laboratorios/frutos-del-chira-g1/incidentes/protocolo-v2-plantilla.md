# Frutos del Chira — Protocolo V2 (PLANTILLA, NO COMPLETADO)
**A completar SOLO tras el Evento 2 del docente.** No reemplazar la versión V1.

## Evento 2 recibido literalmente
> [PEGAR MENSAJE DEL DOCENTE CON HORA]

## Línea de tiempo del fallo
| Hora/Minuto | Hecho observado | Dispositivo/actor | Evidencia |
|---|---|---|---|
| ... | ... | ... | ... |

## Causa raíz y brecha de V1
- ¿Qué alerta se perdió o llegó tarde? ...
- ¿Qué paso del protocolo no se cumplió? ...
- ¿Falla de tecnología, persona, detección o confirmación? ...

## Cambios concretos V1 -> V2
| Sección | Antes | Después | Responsable | Evidencia de prueba |
|---|---|---|---|---|
| Comunicación / ACK | ... | ... | ... | ... |
| Escalamiento/RACI | ... | ... | ... | ... |
| Recuperación | ... | ... | ... | ... |

## Prueba de corrección (ejecutar)
1. Generar alarma reproducible; registrar `event_id` y hora.
2. Interrumpir la condición exacta del evento docente.
3. Verificar estado GUARDADA / ENVIADA / RECIBIDA / RECONOCIDA.
4. Comprobar escalamiento y canal alternativo real o simulado claramente identificado.
5. Validar que no se marcan ACK falsos ni se borran mensajes pendientes.
6. Capturar resultado y responsables.

## Lecciones aprendidas
- Qué cambió en arquitectura V2: ...
- Qué cambió en rol y procedimiento: ...
- Qué se vigilará a futuro: ...

**Estado:** pendiente del evento y de la verificación.
