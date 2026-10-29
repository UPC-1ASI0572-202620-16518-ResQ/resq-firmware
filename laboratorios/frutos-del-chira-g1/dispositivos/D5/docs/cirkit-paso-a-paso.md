# Construcción individual D5 en Cirkit Designer (trabajo compartido)
**Responsable:** Fabricio Fabian Quispe Barzola. **Condición:** entrar solo cuando Gonzalo/integrante 4 entregue el turno, sin borrar D1–D4.

## A. Antes de tocar el proyecto
1. Comprobar visualmente D1, D2, D3 y D4 y sus nombres.
2. Guardar captura de arquitectura V1 y enlace/versión del tapiz; si Cirkit lo permite, duplicar antes de editar.
3. Comprobar con responsables de Z4/Z6 la decisión de red: en este paquete se propone RS485 troncal + router WAN dual.
4. Identificar una zona física libre para D5 y otra zona conceptual para router, nube y enlaces.

## B. Insertar componentes
5. Añadir ESP32 DevKit V1, etiqueta `D5 – Gateway/Perímetro`.
6. Añadir cuatro JSN-SR04T; separar visualmente canales U1...U4 y sus transductores.
7. Añadir dos FC-37 de lluvia; etiquetar R1/R2.
8. Añadir dos flotadores; etiquetar F1/F2.
9. Añadir un DS3231 I2C, una tarjeta/módulo microSD SPI y (si se representa) una MCP23017 para flotadores.
10. Añadir cuatro divisores para ECHO (cada uno 10 kΩ serie arriba + 18 kΩ a GND desde GPIO), RS485 3.3V **preferible aislado** con DE/RE, regulador y protectores adecuados.
11. Añadir visualmente router Ethernet+4G con SIM, caja IP65 y respaldo eléctrico como **elementos propuestos, no requeridos como sensores**.

## C. Cablear según tabla de pines en `03_Mapa_GPIO_y_BOM.md`
12. Conectar U1..U4 TRIG a GPIO 13/14/25/26. Conectar ECHO mediante **divisor** a GPIO 32/33/34/35; nunca conectar 5 V directamente a entradas de ESP32.
13. Dar 5 V a módulos JSN y GND común de sus fuentes (si no hay aislamiento); verificar corriente de la fuente. Instalar la electrónica del sensor en caja seca: solo el transductor es impermeable.
14. Alimentar FC-37 a 3.3 V solo si la revisión de versión confirma salida AO en rango 0–3.3 V; conectar AO a GPIO36/39 (ADC1). Calibrar polaridad de humedad.
15. Conectar DS3231 I2C (SDA21/SCL22), alimentado en 3.3 V, comprobando que sus resistencias pull-up sean también a 3.3 V.
16. Conectar MCP23017 en el mismo I2C, dirección 0x20; F1/F2 a GPA0/GPA1 con pull-up a 3.3 V (contacto a GND). Si no está disponible en Cirkit, etiquetar interfaces lógicas sin fingir un cableado funcional.
17. Conectar microSD (SPI): CS5, SCK18, MISO19, MOSI23; módulo comprobado compatible con 3.3V. GPIO5 es pin de arranque: evitar circuito que altere el nivel en reset.
18. Conectar RS485 de campo (UART2 TX17 RX16, DE/RE27) al bus de comunicación de D1..D4. Usar transceptor con VIO=3.3V, GND/aislamiento correctos y terminaciones solo en extremos.
19. Dibujar router, WAN principal y LTE de respaldo como red **lógica**, no alimentar circuitos desde el símbolo WAN.
20. Etiquetar las rutas: `D1-D4 → BUS RS485 → D5 → WiFi AP del router dual WAN → Internet/4G → nube`.

## D. Comprobar y entregar
21. Revisar fuentes de 5/3.3V, retornos y señales; no conecte ECHO sin divisor ni salidas de FC-37 de 5V al ESP32.
22. Revisar topología RS485 **en línea** y el bus propio Modbus de D4 independiente.
23. Comprobar que están los cinco bloques D1–D5, las etiquetas y la nube; prohibición LoRa.
24. Guardar captura de D5 en detalle y captura del proyecto completo; registrar qué está representado conceptualmente.
25. Enviar al grupo enlace de Cirkit y documentos de D5. El profesor pide tapiz Z1–Z10, no solo circuito.

## E. Precauciones
- Los números son una **propuesta** para ESP32 DevKit V1; confirmar esquema real, variante de sensor y componentes disponibles en Cirkit antes de cablear.
- Una UPS/batería exige cálculo de cargas, potencia y autonomía; no declarar que 72 h están cubiertas.
- Prohibido LoRa/LoRaWAN para el sistema crítico de este caso.
