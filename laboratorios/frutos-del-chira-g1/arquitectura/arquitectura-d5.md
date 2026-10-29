# D5 — Arquitectura del gateway
Responsable: **Fabricio Fabian Quispe Barzola**.

## 1. Función y alcance
D5 concentra los cuatro nodos D1–D4 por red interna **RS485 multipunto de dos hilos**, y además mide 4 niveles con JSN-SR04T, 2 puntos de lluvia con FC-37 y 2 flotadores. Conserva RTC y microSD. **No utiliza LoRa/LoRaWAN**. Un enlace WAN principal (internet ya pagado) se apoya en 4G de respaldo. Se recomienda un router dual WAN/failover con Ethernet WAN + LTE + Wi-Fi AP de acceso para D5; debe verificarse disponibilidad y cobertura. El centro remoto y el cliente dependen de WAN; vigilancia local y registros en microSD dependen de alimentación y sensores.

## 2. Arquitectura lógica
```
D1 Cámaras C1/C2 ─┐
D2 Cámaras C3/C4 ─┤  RS485 multidrop (D1...D5, topología bus;
D3 Túnel/andén  ─┤  terminación 120 ohm SOLO extremos,
D4 Sala máquinas ─┘  polarización controlada, transceptores aislados)
                  ↓
             [D5 ESP32] ← 4 JSN-SR04T + 2 FC-37 + 2 flotadores
               │   ├─ I2C 3.3V: DS3231 + MCP23017
               │   └─ SPI 3.3V: microSD; logs / ACKs / secuencia
               ↓
       [Router dual WAN con Wi-Fi AP]─WAN1: Internet existente
               └───────────────LTE: 4G con SIM de respaldo
               ↓
       Nube / tablero / centro de Piura / importador EE. UU.
```
**Crítico:** El bus que D4 usa para sus PZEM Modbus NO es el bus D1–D5: requiere interfaz y UART separados o pasarela propia. Las conexiones lógicas del tapiz no son conexiones de alimentación.

## 3. Protocolo de nodos propuesto (coordinar con equipo)
- Nodo: D1, D2, D3, D4, D5.
- Mensaje: `device_id`, `boot_id`, `seq`, `timestamp_utc`, `type`, `priority`, `state`, `sensor_summary`, `crc`, `auth_tag`.
- RS485: direccionamiento por gateway maestro y sondeo por turnos; evitar colisiones. Baudrate inicial 9600/19200, paridad y longitud por acordar. CRC para errores accidentales + HMAC para autenticación **si se implementa llave precompartida por nodo**. Un CRC solo no es protección criptográfica.
- Alertas: capacidad de interrupción lógica en el protocolo (gateway consulta cola urgente frecuentemente); RS485 half-duplex no permite que todos transmitan arbitrariamente al mismo tiempo.
- Heartbeat propuesto: cada 30 s; alarma por 3 latidos perdidos (>90 s), sujeto a validación.
- Antirrepetición: par (`device_id`,`boot_id`,`seq`) con contador monotónico persistente y ventana limitada; si un nodo reinicia, `boot_id` debe seguir siendo único. Cifrado de aplicación opcional con AES-GCM y manejo de llaves; TLS para WAN.

## 4. Store-and-forward / consistencia
1. Validar lectura y mensaje (rango, sensor, secuencia, reloj, checksum y MAC si está habilitado).
2. Escribir registro `event_id|device_id|prio|timestamp|tipo|datos|checksum` en `events.log` y confirmar flush/cierre. Si no hay escritura válida, elevar FALLA_SD e informar localmente.
3. Mantener una cola *durable* en SD, no únicamente en RAM. Al reiniciar, volver a leer y descartar filas incompletas con checksum incorrecto.
4. Seleccionar eventos NO confirmados por prioridad: P0 crítica (inundación / temperatura grave / energía); P1 alta (fallas de bus, sensor, nodo); P2 normal (resúmenes / heartbeat); P3 histórico (reenvíos antiguos).
5. No permitir que una sincronización histórica masiva impida eventos críticos nuevos: alternar prioridad urgente y lotes históricos con cuota y backoff.
6. Transmitir por WAN principal o 4G; exigir ACK de aplicación del servidor, que incluya ID de evento. HTTP 200 sin identificación de evento NO garantiza persistencia idempotente.
7. Persistir ACK en `acks.log`; luego marcar el evento como sincronizado. Si falla el guardado del ACK, reenviar con MISMO ID. El servidor deduplica por clave única del evento.
8. Monitorizar espacio SD (alarma por capacidad y plan de retención); copiar fuera de planta cuando vuelva WAN. Una SD sola no reemplaza un respaldo en nube.
9. Ajustar reloj RTC por una fuente autenticada al regresar WAN, anotando cualquier salto de hora. No sobrescribir timestamp original sin rastro.
10. La tarjeta y RTC necesitan energía. Para tres días sin WAN pero con energía, guardar localmente; ante corte total se requiere UPS/batería dimensionada (8 h de generador de planta no garantiza operación 72 h).

## 5. Estados: separar riesgo de salud del sistema
- `NORMAL`: ninguno de los umbrales de riesgo confirmado; sensores válidos.
- `ALERTA`: lluvia activa sostenida, nivel creciente o condiciones de aviso.
- `CRÍTICO`: flotador crítico, distancia de agua bajo umbral de montaje o inundación confirmada.
- `FALLA_SENSOR`: datos no válidos o sensores perdidos sin condición física crítica conocida.
**Flags en paralelo:** WAN1, WAN2, SD, RTC, cada D1–D4, cantidad de cola y alimentación. Una inundación y una WAN caída pueden coexistir.
- `watchdog`: habilitar en firmware de campo tras asegurar que las operaciones largas no bloqueen; pruebas de reinicio y recuperación. Wokwi incluye bucle acotado pero no valida un watchdog de campo.

## 6. Consumo WAN y almacenamiento (supuesto, no dato de la guía)
Si los 5 nodos agregan un resumen de 120 B cada 5 min: `5 × 288 × 120 × 30 = 5,184,000 B = 5.184 MB/mes` (sin cabeceras, ACK, alertas, cámaras). Envío crudo de 52 sensores cada minuto y 64 B: `52 × 1440 × 64 × 30 = 143,769,600 B = 143.7696 MB/mes`. Ahorro de carga útil `96.394%`.
- Buffer de 72 h: `5 × 288 × 120 × 3 = 518,400 B` (~0.52 MB), antes de logs, metadata, alertas y sobrecarga. Dimensionar por lo menos **5 MB lógicos** para telemetría agregada con un factor x10 de margen; recomendable tarjeta >= 8 GB por disponibilidad, protección adicional y reserva (no por necesidad de esos 0.52 MB).
- Mensajes periódicos que dependan del gateway y/o cloud: agregar heartbeat y bytes de protocolo a las cifras definitivas que calculará Z6.
- Video: este cálculo NO incluye streaming; el caso exige vigilancia nocturna por internet, por lo cual la WAN principal debe dimensionarse por video. Si falla, no prometer video continuo por LTE de bajo costo.

## 7. Fallos y continuidad
| Falla | Se mantiene | Se pierde o degrada | Mitigación |
|---|---|---|---|
| WAN principal caída, 4G activo | Nodos, SD, nube con poco tráfico | Video de calidad completa | Priorizar alarmas/resúmenes |
| Ambas WAN caídas (72 h) | Nodos y SD **con energía** | Monitoreo remoto, datos nuevos al importador | Alertas locales, cola y reenvío; lote pasa a observación >2 h |
| SD defectuosa | Alarmas locales y lectura | Auditabilidad persistente | S4/S3 según criticidad, mantenimiento, reserva de memoria limitada |
| Red eléctrica perdida | Solo equipos con respaldo real | Servicios sin energía | UPS calculada y generador sujeto a disponibilidad |
| RS485 troncal cortado | D5 perímetro local | D1–D4 aislados de gateway | Repuestos, segmentar bus, modo local por nodo |
| Router destruido | D5 local/SD si tiene energía | WAN | Reemplazo, segunda ruta solo si existe hardware independiente |

## 8. Plan posterior al evento docente
Al minuto 38, registrar daño concreto y construir arquitectura V2 en Z4 sin borrar V1. Al minuto 80, registrar causa-raíz de falla de protocolo, agregar ACK humano/escalamiento verificable y documentar V2, pero nunca inventar el fallo real.
