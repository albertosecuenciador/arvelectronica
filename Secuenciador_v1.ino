/* 
 * Secuenciador simple
 *
 * 17/07/2017
 *
 * ejemplo para blog de ARVElectronica
 *
 * 4x4 Teclado matricial conectado a la Arduino
 * Este código envía una nota la puerto MIDI cada vez que se 
 * Presiona un número en el teclado.
 *
 * alberto@arvelectronica.com.ar
 *
 */

#include <Keypad.h>

const byte numFilas= 4; // número de filas en el teclado
const byte numColumnas= 4; // número de columnas en el teclado

//mapaTeclado define la tecla presionada de acuerdo a la fila y la columna tal cual aparecen en el teclado de membrana.

char mapaTeclado[numFilas][numColumnas]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

// este código muestra la conexión del teclado a la Arduino

byte pinesFila[numFilas] = {31,33,35,37}; // Filas 0 a 3
byte pinesColumna[numColumnas]= {39,41,43,45}; // Columnas 0 a 3

// Inicializa una variable de clase Keypad

Keypad miTeclado= Keypad(makeKeymap(mapaTeclado), pinesFila, pinesColumna, numFilas, numColumnas);


//  Ejecuta una nota MIDI.  No valida para ver
//  si mensajeMIDI es mas grande que 127, o que los demas valores sean menores que 127:

void activaNota(int mensajeMIDI, int tono, int volumen) {
  Serial2.write(mensajeMIDI);
  Serial2.write(tono);
  Serial2.write(volumen);
}




void setup()
{

 // Iniciamos puerto serie para ver msj en el monitor serial...
 
  Serial.begin(9600);


 //  Configura la velocidad de transmisión del puerto MIDI:
 
  Serial2.begin(31250);
  
  
  
  // Ejecuta notas desde F#-0 (0x1E) a F#-5 (0x5A), una sola vez :
  for (int note = 0x1E; note < 0x5A; note ++) {
    //Nota en canal 1 (0x90), valores de notas (note), volumen intermedio (0x45):
    activaNota(0x90, note, 0x45);
    delay(100);
    //Nota en canal 1 (0x90), valores de nota (note), volumen intermedio (0x00):
    activaNota(0x90, note, 0x00);
    delay(100);
  }



}

//Si se presiona una tecla es almacenada en la variable 'tecla_presionada' 
// Si la teclano es igual a 'NO_KEY', entonces se muestra esta tecla.
// Si la variable count=17, entonces se le asigna cero (Significa que no se oprimió una tecla durante el escaneo del teclado ).

void loop()
{

// Iniciamos bucle eterno...
  
char tecla_presionada = miTeclado.getKey();
if (tecla_presionada != NO_KEY)
{
Serial.print(tecla_presionada);

int note = 0x1E;

switch(tecla_presionada) {

case '1':

note += 2;
break;
case '2':
note += 4;
break;
case '3':
note += 6;
break;
case '4':
note += 8;
break;
case '5':
note += 10;
break;
case '6':
note += 12;
break;
case '7':
note += 14;
break;
case '8':
note += 16;
break;
case '9':
note += 18;
break;
case '*':
// Iniciar secuencia, en próximas versiones
break;
case '0':
// Borrar secuencia
break;
case '#':
// Detener secuencia
break;

default:

break;
  
}

activaNota(0x90, note, 0x45);
    delay(100);
    //Nota en canal 1 (0x90), aquí van los valores de las notas, volumen nulo (0x00):
    activaNota(0x90, note, 0x00);
    delay(100);


}
}


// Cone este código, una vez oresionada una tecla, se muestra en el monitor serie de Arduino, y se envía al puerto MIDI, después que se haya compilado y cargado el código en la placa.
