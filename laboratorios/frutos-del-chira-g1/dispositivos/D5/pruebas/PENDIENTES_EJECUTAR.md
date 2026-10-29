# Últimas evidencias que debe generar Fabricio en Wokwi

1. Abrir el proyecto y reemplazar `sketch.ino` por el archivo actualizado de esta carpeta.
2. Ejecutar y capturar `microSD=OK RTC=OK` **y** `[WDT] ACTIVO`; no usar capturas anteriores para demostrar el nuevo código.
3. Monitor Serie: `WAN=0`, `NODE=D1`, `EVENT=CRIT`, `STATUS` (uno por uno); tomar captura.
4. `WAN=1`; dejar ejecutar y escribir `STATUS`; obtener captura donde se vean ACK posteriores y menor cantidad de pendientes.
5. Cambiar un HC-SR04 a 30 cm; capturar `[ESTADO] ... -> CRITICO`; devolver a 60 cm.
6. Mover lluvia por debajo de 1500 y luego por encima de 2000; verificar que no alterne rápidamente.
7. Simular una falla de lectura (desconectar un Echo o ponerlo fuera de rango en Wokwi); esperar tres lecturas del sensor; capturar `FALLA_SENSOR`.
8. SAVE -> copiar enlace permanente `/projects/NNNN` a `entregables/enlaces-wokwi.md`.
9. Recibir Cirkit compartido de Gonzalo y añadir D5; guardar captura real.
10. Grabar video con rostro, máximo 60 s; el costo 3 años y protocolo V2 se completan tras consenso grupal y Evento 2.

**No subir contraseñas, API keys, ni datos privados.**
