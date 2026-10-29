# G1 — Laboratorio IoT Caso A: Frutos del Chira S.A.C.

Material del **laboratorio académico** (caso ficticio), separado del firmware principal de ResQ.
**Restricción:** no utilizar LoRa ni LoRaWAN.

## Responsables y orden de trabajo en Cirkit Designer

| Orden | Responsable | Dispositivo | Zona principal |
|---|---|---|---|
| 1 | Eduardo | D1 — cámaras C1 y C2 | Z1–Z3 |
| 2 | Pedro | D2 — cámaras C3 y C4 | Z4 y redundancia |
| 3 | Fernando | D3 — túnel y andén | Z6 y WAN |
| 4 | Gonzalo | D4 — sala de máquinas | Z7 y Z9 |
| 5 | Fabricio Fabian Quispe Barzola | D5 — gateway y perímetro | Z8, Z10 e integración final |

**Turnos de edición:** Eduardo → Pedro → Fernando → Gonzalo → Fabricio. Cada responsable recibe un único proyecto de Cirkit Designer, incorpora su dispositivo y lo entrega al siguiente sin modificar innecesariamente los bloques previos.

**Convención de carpetas:** usar solo `D1`, `D2`, `D3`, `D4` y `D5`; los nombres de responsables van dentro de README y registros, no en los nombres de carpetas.

## Archivos disponibles por ahora
- [`dispositivos/D5/`](dispositivos/D5/): código de demostración Wokwi, `diagram.json`, dependencias y guías.
- [`arquitectura/arquitectura-d5.md`](arquitectura/arquitectura-d5.md): diseño propuesto.
- [`incidentes/protocolo-v1.md`](incidentes/protocolo-v1.md): borrador de protocolo de incidentes V1 para revisión grupal.
- [`incidentes/protocolo-v2-plantilla.md`](incidentes/protocolo-v2-plantilla.md): **plantilla**, no resultado del Evento 2.
- [`tablas/ia-z10.md`](tablas/ia-z10.md): decisiones preliminares sobre IA, reglas y estadística.
- [`costos/costos-d5.csv`](costos/costos-d5.csv): costos parciales del nodo D5; no representa costo grupal aprobado.
- [`entregables/enlaces-wokwi.md`](entregables/enlaces-wokwi.md): enlaces y estado de evidencias.

## Evidencia D5 disponible hasta ahora
En capturas compartidas se aprecia que el ESP32 inicia, detecta microSD y RTC, registra `[STORE]`, simula mensajes `[TX-SIM]` y `[ACK-SIM]`, y conserva mensajes pendientes durante `WAN=0`. También aparece una orden de restauración `WAN=1`. Falta una captura concluyente **posterior al restablecimiento** mostrando el reenvío de todos los pendientes y su confirmación.

La WAN/ACK remotos, los mensajes RS485 D1–D4 y algunos sensores están **emulados**: no confundir con una integración física o conectividad real. Las pruebas del watchdog y de reinicio necesitan evidencia adicional.

## Pendientes del equipo
- Subir los archivos y evidencias de D1–D4 cuando cada responsable los entregue; las carpetas actuales son marcadores de posición.
- Adjuntar arquitectura V1 y V2 después del Evento 1 real.
- Confirmar costos totales A/B/C y cálculo de MB/mes con hipótesis y fuentes.
- Revisar protocolo V1, corregirlo con el Evento 2 y guardar `protocolo-v2.md` con cambios verificados.
- Compartir los cinco enlaces públicos de Wokwi y las grabaciones individuales.
- Completar tapiz Z1–Z10 y referencias.
