/* 
 * Proyecto ARVElectronica
 *
 * Re compilado el 24/06/2017
 *
 * Modificado el 17/7/2017
 * 
 * Por Alberto Villafañe
 *
 * Traduccion de comentarios :  Joel Villafañe
 *
 * Este proyecto de secuenciador esta aumentado para explicar por partes en dos post sucesivos
 *
 * Incluye cache de notas para generar la secuencia
 * 
 * Y funciones para ejecutar , borrar, detener la secuencia...
 *
 * Por consultas escribir a :  alberto@arvelectronica.com.ar
 *
 * Version 1.5
 *
 */

#include <Keypad.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

// Area de definiciones:


#define I2C_ADDR    0x27 // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
#define BL_pin  3

// Longitud Secuencia

#define MAX_SECUENCIA 64
#define MAX_RETARDO  150

// Vocales con acento

#define ATILDE     \xA0
#define ETILDE     \x82
#define ITILDE     \xA1
#define OTILDE     \xA2
#define UTILDE     \xA3
#define NTILDE     \xA4
#define MNTILDE    \xA5
#define MATILDE    \xB5
#define METILDE    \x90
#define MITILDE    \xD6
#define MOTILDE    \xE0
#define MUTILDE    \xE9

//MIDI Messages


#define NOTEOFF           0x80     // Arrancamos en canal 1 : B10000000
#define NOTEON            0x90
#define AFTERTOUCH        0xA0
#define CONTROLCHANGE     0xB0
#define PROGRAMCHANGE     0xC0
#define CHANNELPRESURE    0xD0
#define PITCHBEND         0xE0
#define SYSEX             0xF0
#define ENDSYSEX          0xF7

//Creamos diferentes caracteres

uint8_t campana[8]  = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
uint8_t figura[8]  = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
uint8_t reloj[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
uint8_t corazon[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
uint8_t pato[8]  = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
uint8_t tilde[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
uint8_t equis[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
uint8_t intro[8] = { 0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
uint8_t sonrisa[8] = {   B00000,   B10001,   B00000,   B00000,   B10001,   B01110,   B00000, };


// buffer de memoria para almacenar la secuencia

int secuencia[MAX_SECUENCIA];
int posicion_secuencia = 0;
int retardo = 100;
int pasos_retardo = 10;
boolean detener_secuencia = false;
int programa = 0;
int n = 1;
   
 // LiquidCrystal_I2C lcd(0x27,16,2);  // definimos la direccion de la pantalla LCD a 0x20 e indicamos el numero de columnas y filas de la pantalla
 
 LiquidCrystal_I2C	lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin, BL_pin, POSITIVE);
 
 //LiquidCrystal_I2C lcd (0x3F, 2, 1, 0, 4, 5, 6, 7, 3);
//lcd.begin (16.2);
 


const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//  keymap define la tecla presinada de acuerdo  a la fila y columna con el mismo orden en que aparecen en el teclado de membrana...

char keymap[numRows][numCols]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

//Codigo que muestra las conexiones del teclado a los pines de Arduino...

byte rowPins[numRows] = {31,33,35,37}; // Filas    0 a 3
byte colPins[numCols] = {39,41,43,45};  // Columnas 0 a 3

// Inicializa una instancia de la clase Keypad

Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

// Enviar SysEX

void enviarSysEx(char * mensajeSysEx ) {

  int comando = SYSEX;
  char *p;
// Inicia System Exclusive

Serial2.write(comando);

p = mensajeSysEx;

while( *p != 0 ) Serial2.write(*p++);


// Fin System Exclusive
comando = ENDSYSEX;

Serial2.write(comando);

    
  }
  
  
  
void instrumento( int cmd, int instrumento )  {
 
  Serial2.write(cmd);
  Serial2.write(instrumento);
 
 
 } 
//  Ejecuta una nota Midi. No valida si los parametros estan entre 0 y 127.


void notaActivada(int cmd, int pitch, int velocity) {
  Serial2.write(cmd);
  Serial2.write(pitch);
  Serial2.write(velocity);
}

//Muestra todos los caracteres .  Es local no esta en librerias...

void displayKeyCodes(void) 
{
  int i = 10;
  while (i--) 
  {
    lcd.clear();               //Borramos la pantalla
    lcd.setCursor(0,1);        //Nos posicionamos en la primera columna de la segunda fila
                               //Mostramos los distintos caractares
    lcd.print("Codigos: 0x"); 
    lcd.print(i, HEX);
    lcd.print("-0x"); 
    lcd.print(i+16, HEX);
    lcd.setCursor(2, 2);  
    for (int j=0; j<16; j++)
      lcd.write(i+j);
                        i+=16;
    delay(40);
  }
}
 
 
 //-----------------Agregar Secuencia en Memoria --------------------------

void agregaNota(int nota, int posi) {
 
if ( posi >= 0 && posi < MAX_SECUENCIA ) secuencia[ posi ] = nota;
  
}

//-----------------Ejecuta Nota------------------------------------------

void ejecutaNota( int nota, int retardo ) {

instrumento( PROGRAMCHANGE,  programa);

  //Nota en canal 1 (0x90), valor de la nota (nota), Amplitud nula (0x45):
    
 // Serial.println(nota);
 
 notaActivada(0x90, nota, 0x45);

  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  
    delay(retardo);

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    
    //Nota en canal 1 (0x90), valor de la nota (nota), Amplitud nula (0x00):
    
    notaActivada(0x90, nota, 0x00);
    
    delay(retardo);
  
  
}

// Ejecutamos la secuencia...

void ejecutaSecuencia( boolean sentido) {
 
 int k; 
  
  for( int i = 0; i < posicion_secuencia; i++ ) {
 
    // if( detener_secuencia == true ) break;
    
     char teclapresionada = myKeypad.getKey();
     
     if( teclapresionada == '#' ) break;
     
     if( sentido ) k = i;
     else          k = posicion_secuencia - (i+1);
     
     
    
  ejecutaNota( secuencia[k], (int) retardo );
  
  
  }
  
}

//------------------------Borramos variables-----------------------------

void borraSecuencia() {
  
posicion_secuencia = 0; 

programa = 0;

detener_secuencia = false;

for( int i = 0; i < MAX_SECUENCIA; i++ )  secuencia[i] = 0;  
  
}

//------------------------------------------------------------------------


void setup()
{
  
Serial.begin(9600);

//  MIDI es un puerto serie, pero con otra velocidad: Configuramos entonces los baudios...

  Serial2.begin(31250);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(2000);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second

// Arrancamos con la pantalla LCD de texto...
//

lcd.begin (16,2); //  <<----- Mi LCD es 16x2

 
// Switch on the backlight
lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
lcd.setBacklight(HIGH);
lcd.home (); // A inicio

 lcd.print("ARVElectr\xA2nica");  // Ahh!!! Los acentos...
 //lcd.print("\xA2");
 //lcd.print("nica");
 // Backlight on/off every 3 seconds
 lcd.setCursor (0,1);  // ir al principio de la segunda línea
 lcd.print("Secuenciador");
 lcd.print(n++,DEC);
 
 //lcd.setBacklight(LOW);      // Backlight off
 //delay(3000);
 //lcd.setBacklight(HIGH);     // Backlight on
 //delay(3000);
 

//Le decimos a la pantalla que cree los distintos caracteres que definimos arriba...


 
  lcd.createChar(0, campana);
  lcd.createChar(1, figura);
  lcd.createChar(2, reloj);
  lcd.createChar(3, corazon);
  lcd.createChar(4, pato);
  lcd.createChar(5, tilde);
  lcd.createChar(6, equis);
  lcd.createChar(7, intro);
  
  // lcd.createChar(7, sonrisa);
  
 
  //lcd.home (); // A inicio
 // lcd.write(byte(0));  // escribe campana...
  //lcd.write(1);        // escribe nota negra
  //lcd.write(2);        // escribe reloj...
  //lcd.write(3);        // escribe Corazon
  //lcd.write(4);        // escribe patito
  //lcd.write(5);        // escribe tilde
  //lcd.write(6);        // escribe equis
  //lcd.write(7);        // escribe enter
  // lcd.write(7);      // sonrisa...
  
   
  Serial.println("EMPEZAMOS");
  
  enviarSysEx("ALBERTO"); 

}
// -----------------------Bucle eterno... ----------------------------
void loop()
{



// Proceso de escaneo de teclado...
// ----------------------------------------------------------------------------------
//   Si una tecla es presionada, entonces es almacenada en la variable 'tecla_presionada' 
//   Si la teclano es igual a 'NO_KEY', entonces esta tecla es enviada al puerto serie 1
//   Si count == 17 se vuelve la cuenta a 0 , significa que no hubo tecla presionada durante el proceso total de escaneo de teclado.
//------------------------------------------------------------------------------------------------------------------------------------
  
char tecla_presionada = myKeypad.getKey();

// if (customKey){
//    Serial.println(customKey);

if (tecla_presionada != NO_KEY)
{
  Serial.println(tecla_presionada);

int nota = 0x1E;

if(isdigit(tecla_presionada)) {
  
  lcd.setCursor (0,1);  
 lcd.print("Secuenciando...  ");
  
  
}
switch(tecla_presionada) {

case '1':
 
nota += 2;


break;
case '2':
nota += 4;

break;
case '3':
nota += 6;

break;
case '4':
nota += 7;

break;
case '5':


nota += 9;
break;
case '6':
nota += 11;

break;
case '7':
nota += 13;
break;
case '8':


nota += 15;

break;
case '9':
nota += 17;

break;
case '*':
// Iniciar secuencia, en próximas versiones
 lcd.setCursor (0,1);  
 lcd.print("Ejecutando...  ");
ejecutaSecuencia(true);


break;
case '0':
// Borrar secuencia
 lcd.setCursor (0,1);  
 lcd.print("Borrando...     ");
borraSecuencia();

break;
case '#':
// Detener secuencia

 lcd.setCursor (0,1);  
 lcd.print("Deteniendo...  ");
detener_secuencia = true;


break;

case 'A':
// Detener secuencia

 lcd.setCursor (0,1);  
 lcd.print("Retardo--    ");
 
if(retardo > pasos_retardo ) retardo-= pasos_retardo;


break;


case 'B':
// Detener secuencia
 lcd.setCursor (0,1);  
 lcd.print("Retardo++     ");
if(retardo < ( MAX_RETARDO-pasos_retardo ) ) retardo += pasos_retardo;


break;

case 'C':
// Detener secuencia
//ejecutaSecuencia(false);

 lcd.setCursor (0,1);  
 lcd.print("MultiSecuencia ");
for (int j=0; j<20; j++) {

 char teclapresionada = myKeypad.getKey();
     
     if( teclapresionada == 'D' ) {
     
     programa++;

if ( programa > 127 ) programa = 0;

    }
    
     else if( teclapresionada == 'A' ) {
     
    if(retardo > pasos_retardo ) retardo-= pasos_retardo;

    }
    
    else if( teclapresionada == 'B' ) {
     
     if(retardo < ( MAX_RETARDO-pasos_retardo ) ) retardo += pasos_retardo;


    }

     else if( teclapresionada == '#' ) break; 
     
  
ejecutaSecuencia(false);

}

lcd.setCursor (0,1);  
 lcd.print("Secuenciador1  ");


break;

case 'D':

 lcd.setCursor (0,1);  
 lcd.print("Instrumento");
// Detener secuencia

//ejecutaSecuencia(true);
//ejecutaSecuencia(false);

programa++;

if ( programa > 127 ) programa = 0;



break;



default:

break;
  
}  // Fin switch

  if(isdigit(tecla_presionada)) {
  
 ejecutaNota( nota, MAX_RETARDO );
    
agregaNota( nota, posicion_secuencia++ );  
 
  
}

lcd.setCursor (0,1);  
lcd.print("Secuenciador1  ");

   } // si tecla_presionada no es NO_KEY
}    // Fin loop 

// Con este codigo, luego de presionar una tecla, esta  aparecera en el monitor serial del software de Arduino una vez que el codigo sea compilado y subido a la placa Arduino.


