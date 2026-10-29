# Enlaces de Wokwi y evidencias individuales

| Nodo | Responsable | Enlace | Evidencia | Estado |
|---|---|---|---|---|
| D1 | Eduardo | Pendiente | Pendiente | Sin verificar |
| D2 | Pedro | Pendiente | Pendiente | Sin verificar |
| D3 | Fernando | Pendiente | Pendiente | Sin verificar |
| D4 | Gonzalo | Pendiente | Pendiente | Sin verificar |
| D5 | Fabricio Fabian Quispe Barzola | **PEGAR ENLACE GUARDADO DE WOKWI** | Capturas locales de WAN caída y registros; adjuntar | Prueba parcial observada |

## Prueba mínima D5
1. `WAN=0`, `NODE=D1`, `EVENT=CRIT`, `STATUS`: confirmar mensajes pendientes con `WAN=DOWN`.
2. `WAN=1`, esperar, `STATUS`: capturar mensajes `[TX-SIM]`, `[ACK-SIM]` y reducción de pendientes.
3. Cambiar distancia de un HC-SR04 para probar criterio de nivel crítico y registrar cambio de estado.
4. Documentar diferencias entre sustitutos de Wokwi y sensores reales.
5. Guardar capturas, no subir credenciales ni claves de servicios.

**Nota:** los comandos se escriben en el Monitor Serie del proyecto Wokwi ejecutándose, no en `sketch.ino`.
