# LCY
Robot que le hice a mi abuelita para que pueda recibir mensajes / estar atenta al gas de la cocina :p 

Puede ser impreso con la combinacion de colores que quieras!

## Proceso de desarrollo
El robot tuvo cambios drasticos en el diseño principalmente porque no calcule bien el espacio de mi impresora 3D, asi que el chasis viene dividido en 2 partes (left - right)

La idea es imprimir una de cada pieza y el circuito hacerlo en una perfboard siguiendo las conexiones que estan en el mockup (pines a eleccion libre)

![Implementacion completa](https://github.com/user-attachments/assets/4f4d3131-12fa-44e5-9360-a83d61dce40b)

![Ensamble](https://github.com/user-attachments/assets/6bac4b9b-f0da-4641-b549-c983bb63520b)

![Render_3D](https://github.com/user-attachments/assets/142bac97-2099-473f-9ff1-c1768d5e8217)

![Primer_mock_up](https://github.com/user-attachments/assets/abb3a5d7-7fb2-4988-a328-1b9955b8c6cb)

## Componentes usados
* Sensor MQ-2 de gas
* Buzzer pasivo
* Buzzer activo de 12V
* ESP32 WROOM32 DEVKIT
* Headers / Perfboard / Cables
* Pantalla LCD I2C 20x4
* Switch simple (boton)

## Implementacion
El codigo viene listo a modo de proyecto en PlatformIO, solo se debe crear un nuevo proyecto y copiar la carpeta **/src** junto al archivo **platform.ini**.
Para poder usar la funcionalidad de mensajes, se debe crear un bot el telegram usando BOTFather y reemplazando la id en el archivo **chatbot.h** eso, junto a colocar la clave de tu internet en el archivo **main.cpp**

## Modos

### Idle
Pantalla apagada actualizando mediciones de gas y chequeando mensajes entrantes

### Sensor de gas
Al apretar el boton en modo idle se cambia a este modo: muestra la medicion actual del sensor de gas

### Mensajes
Se activa cuando se recibe un mensaje en el bot de telegram, se activa el mini buzzer y alerta al usuario que enviaron un mensaje, pulsa el boton para marcarlo como "recibido"

### Fuga de gas
Se activa cuando se detecta un nivel anormal de gas en el ambiente, enciende la alarma principal (muy fuerte) hasta que se aprete el boton indicando que se corto el gas o bien cuando los niveles vuelvan a la normalidad.
