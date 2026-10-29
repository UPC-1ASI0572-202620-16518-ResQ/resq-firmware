# G1 — Frutos del Chira S.A.C. — Protocolo de respuesta a incidentes V1
**Coordinador:** Fabricio Fabian Quispe Barzola (D5). **Estado:** propuesta previa a Evento 2; validar con equipo y modificar según Evento 1 y condiciones reales. 
**Fecha de borrador:** 2026-10-29. **Objetivo:** preservar personas, cadena de frío, trazabilidad y continuidad de sensores.

## 1. Activos críticos
| Activo | Pérdida o falla | Respuesta inmediata |
|---|---|---|
| C1–C4 y producto | Excursión térmica/rechazo USD 25,000 por lote (dato ficticio) | Monitoreo redundante, evaluar calidad, restringir aperturas |
| Túnel, andén | Enfriamiento incompleto, temperatura pulpa | Supervisar por lotes y anotar temperaturas |
| Compresores, generador | Sin refrigeración ni energía | Priorizar cargas y aplicar protocolos eléctricos seguros |
| D1–D4 y buses 1-Wire/I2C/RS485 | Lecturas faltantes | Registrar fallo y habilitar ruta redundante si disponible |
| D5, gateway, RTC y SD | Falta de telemetría y evidencia local | Alerta local, verificación física, copias recuperables |
| WAN1 y WAN2 | Sin cámara remota, cliente ni alertas remotas | Registro local y canal humano alternativo |
| Nube y trazabilidad | Sin auditoría fuera de planta | Persistir cola y revalidar reenvío con ACK |

## 2. Severidad S1 a S4 (umbrales de PROPUESTA)
| Nivel | Condición observable | Respuesta objetivo | Quién atiende | Escalamiento |
|---|---|---|---|---|
| S1 Operación | Lecturas normales y heartbeats vigentes | Cada ciclo operativo | Operador turno | Reporte rutinario |
| S2 Alerta | Lluvia persistente 5 min; 1 sensor defectuoso con redundancia válida; WAN1 caída con WAN2 operativa | Revisar <= 15 min | Operador/TI | Jefe planta si persiste 30 min |
| S3 Grave | Temperatura fuera de objetivo durante 10 min (valor concreto a validar por producto); ausencia de D1–D4 >90 s; ambas WAN caídas; falta de trazabilidad >2 h | Acción <= 5 min | Jefe planta/TI/operador | Gerencia si no se recupera |
| S4 Crítico | Flotador crítico, agua junto a energía, compresor crítico detenido sin respaldo, peligro humano, ciberincidente activo con control de equipos | Atención inmediata, aislar solo si seguro | Jefe planta y operador; seguridad/mantenimiento | Gerencia, emergencias según caso |
**No son valores certificados**: la temperatura de almacenamiento objetivo del caso es 0–1 °C y humedad 90–95%; el tiempo tolerable de excursión depende del lote. No imponer reinicio de equipos inundados.

## 3. Roles, RACI y turnos
A=aprueba/autoridad final; R=ejecuta; C=consultado; I=informado. Una actividad puede tener varios ejecutores, pero se asigna un único A.
| Actividad | Operador | Mantenimiento | Jefe planta | TI/IoT | Gerencia | Centro Piura |
|---|---|---|---|---|---|---|
| Detectar y registrar alarma | R | I | A | C | I | C |
| Priorizar refrigeración | R | R | A | C | I | C |
| Aislamiento eléctrico seguro | I | R | A | C | I | I |
| Conmutación WAN y recuperación SD | I | C | A | R | I | C |
| Declarar retorno a operación | C | R | A | R | I | C |
| Aviso a importador/gerencia | I | C | R | C | A | I |

Turno nocturno 22:00–06:00: **no hay técnico de frío**. Opera el personal de turno y el centro remoto si WAN disponible; se notifica mantenimiento de guardia o al responsable establecido. No suponer presencia inmediata de un especialista de frío.

## 4. Comunicación y confirmación
1. WAN principal: internet existente (si disponible). WAN alterna: LTE/4G de router dual WAN si hay cobertura y energía.
2. Alertas S3/S4: registrar ID único local, intentar entrega remota y guardar resultado. El servidor debe confirmar recepción con ID. **Entrega al servidor ≠ atención humana.**
3. El responsable humano confirma ACK operativo con nombre, hora, acción. Si no confirma S4 en 2 min (propuesta), reintentar/escalar al suplente y jefe; seguir intentando canales activos. Si S3 no confirma en 5 min, escalar.
4. Sin WAN, utilizar alarma local y comunicación directa de operadores en planta (radio VHF independiente si la planta lo proporciona; comunicación personal segura). SMS solo cuando la red celular está disponible; no reemplaza una red completamente caída.
5. Preservar estados separados: GENERADA, GUARDADA, ENVIADA, RECIBIDA_SERVIDOR, RECONOCIDA_HUMANO, CERRADA.
6. Los 2 h sin información remota del lote implican ESTADO EN OBSERVACIÓN del cliente. Mostrar última marca de tiempo, no datos antiguos como si fuesen actuales.

## 5. Procedimientos por incidente
### 5.1 Corte de energía
- **Detectar:** medidores D4, heartbeat de energía/router, operador. **Clasificar:** S3 o S4 si compromete refrigeración/personas.
- **Automático:** guardar evento si D5 tiene energía; modo de bajo consumo, prioridades P0/P1. **Personal:** evaluar generador de 250 kVA y autonomía nominal 8 h, revisar cargas y combustible; no asumir 72 h.
- **Escalar:** jefe planta/mantenimiento; gerencia si pérdida continua. **Evidencia:** tiempos, cargas, temperaturas, decisiones. **Recuperación:** inspección eléctrica, retorno escalonado y validación del monitoreo.
### 5.2 Falla de sensor
- **Detectar:** lecturas NAN, fuera de rango, estancadas o discordantes. **Clasificar:** S2 si redundante, S3 si deja cámara sin evidencia.
- **Automático:** FALLA_SENSOR, excluir dato inválido de agregados, registrar `node_id` + sensor + secuencia. **Personal:** revisar cable, alimentación y sensor sin comprometer frío. **Recuperar:** calibrar/verificar contra referencia.
### 5.3 Falla de bus
- **Detectar:** tres heartbeats perdidos, CRC inválido, timeout. **Clasificar:** S3 cuando afecta varias cámaras.
- **Automático:** aislar segmento lógico, continuar alarmas autónomas de nodo y guardar fallos D5. **Personal:** comprobar terminación, cable, transceptor, ruido. **Recuperar:** verificar protocolo, direcciones y paquete de prueba.
### 5.4 Pérdida de WAN
- **Detectar:** heartbeat cloud sin ACK, estado router. **Clasificar:** S2 si LTE funciona, S3 ambas WAN caídas.
- **Automático:** conmutar router (si existe), enviar solo urgentes, guardar SD y mantener RTC; alertas locales. **Personal:** avisar por canal independiente; informar al centro/cliente cuando se restablezca. **Recuperar:** reenvío idempotente con ACK y marcado de retrasos.
### 5.5 Inundación
- **Detectar:** F1/F2, 4 JSN, lluvia y confirmación humana. **Clasificar:** S4 si amenaza personas/electricidad.
- **Automático:** alarma P0 local, registro persistente si hay energía; nunca encender bombas/compresores sin interbloqueos aprobados. **Personal:** aislar zona y energía solo cuando sea seguro, evacuar si procede. **Recuperar:** inspección, secado, aislamiento eléctrico y pruebas.
### 5.6 Tormenta eléctrica
- **Detectar:** aviso meteorológico, sobretensión, pérdida RS485. **Clasificar:** S3/S4 según daños.
- **Automático:** conservar eventos, degradar bus afectado. **Personal:** evitar manipular equipos energizados, inspeccionar SPD/tierra/aislamiento tras evento. **Recuperar:** medir continuidad, reinstalar protecciones y probar D1–D5.
### 5.7 Ciberincidente
- **Detectar:** mensajes duplicados/repetidos, firmas inválidas, credenciales comprometidas. **Clasificar:** S3/S4 según impacto.
- **Automático:** rechazar tramas sin autenticación y registrar intentos sin bloquear alarma local. **Personal:** segmentar acceso WAN/contener credenciales con procedimiento seguro; preservar evidencia original. **Recuperar:** rotar claves, corregir, validar secuencias, desplegar gradualmente y revertir si falla.
### 5.8 Falta de repuestos
- **Detectar:** stock bajo, plazos de importación. **Clasificar:** S2/S3 según pérdida de redundancia.
- **Automático:** marcar degradado y priorizar telemetría crítica. **Personal:** inventario de repuestos equivalentes, conservación segura de producto; no sustituir partes sin compatibilidad eléctrica. **Recuperar:** validación de nueva pieza y registro de trazabilidad.

## 6. Evidencia y almacenamiento
Registro mínimo: `event_id`, `device_id`, `boot_id`, `sequence`, UTC, hora RTC válida/inválida, prioridad, estado, valores, alarmas, resultados WAN, ACK servidor y ACK humano. Escribir primero SD y luego transmitir; persistir ACK de aplicación; servidor deduplica por ID. Mantener versionado, exportaciones firmadas/autorizadas, control de acceso, respaldo fuera de planta cuando WAN retorna. Comprobar corrupción de registros en reinicio; alertar espacio libre. Si SD falla no afirmar auditabilidad. Las fotos/videos del centro de Piura dependen de WAN y capacidad del enlace.

## 7. Recuperación y reversión
**Precondiciones:** seguridad física, energía estabilizada, sensores, buses, gateway, RTC, SD y ambos enlaces probados; todas las alertas abiertas revisadas. Restaurar primero alarmas locales y seguridad, luego telemetría, después tableros. Conciliar pendientes por ID y marcas de tiempo. Si un cambio causa problemas, restaurar firmware/configuración anterior registrada, revisar logs y no borrar el historial. Jefe de planta autoriza retorno a modo normal; TI y mantenimiento documentan pruebas.

## 8. Lecciones aprendidas y V2
**Aún pendiente de recibir el Evento 2 real.** Completar únicamente con hechos del docente: falla concreta, causa raíz, mecanismo de detección o ACK que no funcionó, versión corregida, validación, responsables, fecha y cambios en arquitectura. Mantener V1 intacta y crear `Protocolo_V2.md` después del evento.

**Verificación del grupo:** D1 revisa temperaturas y votación, D2 prioridades eléctricas/lotes, D3 túnel y resúmenes, D4 Modbus/compresores, D5 WAN/almacenamiento/escala y comunicación.
