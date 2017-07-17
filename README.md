Blog www.arvelectrónica.com.ar

Para el post #1 descargar el archivo: secuenciador_v1.ino

# arvelectronica
Proyectos para promover la música electrónica a través del software libre

El secuenciador tiene como base una placa Arduino MEGA 2560, un display LCD de 16x2 operado con un conversor I2C a paralelo conectador a los puertos 20 y21, un teclado matricial de membrana de 16 teclas en los puertos 31,33,35,37,39,41,43,45, y una placa MIDI conectada en el puerto Serial 2 de la MEGA, pines 16 y 17. El esquema de manejo es el siguiente:

- Números: Ejecuta una nota y la guarda en memoria.
- Asterico: Ejecuta la secuencia guardada.
- 0: Borra la secuencia de la memoria
- Numeral: Detiene la secuencia en ejecución
- A: disminuye tiempo entre notas.
- B: Aumenta tiempo entre notas
- C: Ejecuta la secuencia al revés 20 veces.
- D: Cambia el programa ( Instrumento MIDI 1-128 )


Cualquier inquietud , sugerencia, etc. no duden en contactarme:

alberto@arvelectronica.com.ar

Contribuciones: Pueden ser en forma de mejoras al código, o monetariamente al siguiente link:

https://www.paypal.me/arvelectronica

Por otros usos que no sean personal o como hobbista solicitar los permisos correspondientes




