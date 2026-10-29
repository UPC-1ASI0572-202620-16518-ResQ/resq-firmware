# D5 — Fabricio Fabian Quispe Barzola: pregunta, sustento y video

## Pregunta exacta del caso
«¿Qué WAN principal y de respaldo eligió sin LoRa y dentro de S/ 150 al mes? ¿Qué ven el cliente y el centro de monitoreo en modo degradado, y cómo corrige el protocolo versión 2 la alerta perdida? ¿Qué alternativa (A, B o C) eligió el grupo y cuánto cuesta a 3 años?»

## Respuesta técnica desarrollada (se completará al recibir resultados reales del equipo)
- **WAN principal propuesta:** internet de la planta ya pagado (costo incremental S/0 para el caso; su supervivencia ante lluvias no está garantizada). **Respaldo propuesto:** LTE/4G mediante router dual WAN con plan mensual que debe verificar cobertura real en Sullana. **Presupuesto recurrente:** elegir plan 4G de **hasta S/150 mensuales por sitio**; propuesta administrativa S/110/mes (no es cotización verificada), por tanto S/110 <= S/150. No confundir inversión inicial del router con costo mensual del plan.
- **Tráfico:** 5 nodos × 288 resúmenes/día × 120 B × 30 = 5.184 MB/mes de cargas útiles; crudos: 52×1440×64×30=143.770 MB/mes. Sin streaming de cámaras y sin sobrecarga de red. El operador Z6 debe validar tamaño de paquetes y presupuestar video nocturno por el enlace principal.
- **Modo degradado con LTE:** alertas, últimas temperaturas y estado resumido; video continuo puede quedar suspendido por cuota/cobertura.
- **Sin ambas WAN:** D5 y nodos mantienen alarmas locales y SD **si tienen alimentación**; el centro de Piura no recibe cámaras ni nuevas telemetrías, y el cliente ve únicamente datos históricos con hora visible, no tiempo real. El lote queda en observación si pasa >2 h sin información.
- **Recuperación:** D5 guarda evento+UTC+secuencia+prioridad, reintenta al regresar WAN y solo registra confirmado tras ACK remoto con ID; deduplicar en servidor.
- **Protocolo V2:** no afirmar una solución definitiva sin conocer Evento 2. Propuesta si el problema es alerta perdida: ACK de aplicación + confirmación humana + escalamiento y canal alternativo, con prueba.
- **Alternativa A/B/C:** pendiente de que Gonzalo / Z9 confirme cuál eligieron y el total a 3 años. Ejemplo grupal PRELIMINAR no definitivo: B S/19,160 solo si Z9 aprueba explícitamente las hipótesis de inversión, operación y riesgo del primer análisis.

## Guion de video <= 1 minuto — plantilla con campos pendientes
«Soy Fabricio Fabian Quispe Barzola, responsable del dispositivo D5, gateway y perímetro. Propusimos internet existente como enlace principal y 4G como respaldo, con un gasto mensual de [COSTO REAL APROBADO] soles, dentro del límite de 150. Nuestro diseño reduce la telemetría a aproximadamente 5.18 megabytes mensuales de carga útil. Si fallan ambas redes, las alarmas y la tarjeta SD siguen solo si existe alimentación; el cliente observa el último registro y sus lotes entran en observación después de dos horas. Al volver internet, enviamos eventos por prioridad y confirmamos su recepción. Tras el Evento 2, corregimos [FALLO REAL Y MEDIDA VERIFICADA]. Elegimos la alternativa [A/B/C], cuyo costo total estimado a tres años es [MONTO APROBADO] soles».

**Grabación:** mostrar D5 en tapiz Cirkit o Wokwi, cámara encendida y voz propia; nombre `G1-D5-Quispe.mp4` (el caso solicita estructura `G1-D<n>-<apellido>.mp4`). Ensayar con cronómetro: reducir frases si excede 60 s.

## Exposición / preguntas frecuentes
1. **¿Por qué no LoRa?** Porque está prohibido por la auditoría del caso; usamos RS485 local más internet/4G con autenticación y secuencia, sin afirmar inmunidad absoluta a ataques.
2. **¿Por qué 4 ultrasonidos?** Redundancia espacial y monitoreo de cuatro puntos de riesgo de agua; un JSN mide distancia al agua, que se convierte a nivel respecto a altura calibrada.
3. **¿Cómo funciona D5 72 h sin WAN?** Solo con energía y SD disponibles; guarda cola local y se pierde visualización remota.
4. **¿Qué protege el ECHO 5V?** Divisor resistivo 10k/18k (o conversor lógico), evitando 5V directo al GPIO ESP32.
5. **¿Cómo evitas duplicados?** ID compuesto, secuencia y ACK servidor; retransmitir con mismo ID.
6. **¿Qué pasa si falla la SD?** Falla auditabilidad; se informa FALLA_SD y se preservan alarmas locales mientras la energía lo permita.
7. **¿Se demostró el modem 4G real?** No: Wokwi emula WAN/ACK; la validación física y cobertura es una fase distinta.
