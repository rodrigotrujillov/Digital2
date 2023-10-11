//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#include <SPI.h>
#include <SD.h>

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

//***************************************************************************************************************************************

// IMAGENES O ANIMACIONES ------------------------------------------------------

// Personajes o enemigos
extern uint8_t iceclimber1 [];
extern uint8_t iceclimber2 [];
extern uint8_t Dino [];
extern uint8_t Bug [];
extern uint8_t Bird [];
extern uint8_t iceman [];

// Elementos
extern uint8_t martillito []; // martillo para el menu

// Bloques
extern uint8_t block [];
extern uint8_t rect [];
extern uint8_t block_green [];
extern uint8_t block_brown [];
extern uint8_t rect_green [];
extern uint8_t rect_brown [];

// SD ---------------------------------------------------------------------------
File myFile;
File root;

// VARIABLES PARA PROTOTIPOS ----------------------------------------------------
uint8_t maps[1000];
uint8_t contador = 0;

// BANDERAS DE ESTADOS ----------------------------------------------------------
int banderaPantalla = 0; // bandera para controlar en que pantalla estoy
int banderaMartillito = 0; // bandera para controlar posicion de martillito
int banderaInGame = 0; // bandera para saber si nos encontramos jugando y no en el menu
int banderaUnJugador = 0; // bandera para saber si estamos jugando como un solo jugador
int banderaDosJugador = 0; // bandera para saber si estamos jugandos como dos jugadores 

// DATOS DE JUGADORES -----------------------------------------------------------
int IceClimber1_x = 145; // posicion inicial en X de ice climber 1
int IceClimber1_y = 190; // posicion inicial en Y de ice climber 1
int IceClimber1_width = 16; // ancho ice climber 1
int IceClimber1_height = 24; // altura ice climber 1

int IceClimber2_x = 225; // posicion inicial en X de ice climber 2
int IceClimber2_y = 190; // posiicion inicial en Y de ice climber 2
int IceClimber2_width = 16; // ancho ice climber 2
int IceClimber2_height = 24; // altura ice climber 2

// DATOS DE ENEMIGOS ------------------------------------------------------------
int Dino_width = 35; // ancho dino
int Dino_height = 20; // altura dino
int Bug_width = 35; // ancho bug
int Bug_height = 25; // altura bug
int Bird_width = 20; // ancho bird
int Bird_height = 20; // altura bird 
int iceman_width = 16; // ancho iceman
int iceman_height = 16; // altura iceman

int enemigo_x[15] = {80, 104, 153, 201, 250, 56, 105, 154, 202, 251, 56, 105, 154, 202, 251}; // posiciones X de enemigos (prueba)
int enemigo_y[15] = {35, 35, 35, 35, 35, 71, 71, 71, 71, 71, 104, 104, 104, 104, 104}; // posicion Y de enemigos (prueba)
int enemigo_index[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // index enemigos

bool enemies_move = true; // los enemigos se estan moviendo?

// DATOS DE MAPA ---------------------------------------------------------------
int map_border_left = 73; // borde mapa izquierda
int map_border_right = 230; // borde mapa derecha
int map_floor = 190; // borde del piso

bool LimiteIzquierdaIC_1 = false; // limite izquierda ice climber 1
bool LimiteIzquierdaIC_2 = false; // limite izquierda ice climber 2
bool LimiteDerechaIC_1 = false; // limite derecha ice climber 1
bool LimiteDerechaIC_2 = false; // limite derecha ice climber 2
bool LimiteAbajoIC_1 = false; // limite de piso ice climber 1
bool LimiteAbajoIC_2 = false; // limite de piso ice climber 2

// BLOQUES ---------------------------------------------------------------------
int block_position_x = 64; 
int block_position_y[4] = {168, 120, 72, 24};
int block_random_y[4] = {176, 128, 80, 32};
int blocks = 12;

// OTROS -----------------------------------------------------------------------
int inByte; // byte que recibimos del ESP

int jumpHeight = 20;
bool isJumping = false;
int jumpCounter = 0;

bool collision1 = false; // colision 1
bool collision2 = false; // colision 2


//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  Serial5.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x00);

  SPI.setModule(0);
  Serial.print("Initializing SD card...");
  pinMode(A3, OUTPUT);
  if (!SD.begin(A3)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  std::srand(1234); // para datos random

}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  // INICIAZLIZACION DEL MENU
  while (banderaPantalla == 0 & banderaInGame == 0) {
    LCD_Clear(0x00);
    Serial.println("Iniciando Juego");
    myFile = SD.open("menu.txt"); // extraemos la imagen del menu de la SD
    mapSD();
    LCD_Bitmap(88, 128, 8, 8, martillito); // colocamos el martillito en opcion 1 Jugador
    FillRect(88, 144, 8, 8, 0x00);
    Rect(108, 125, 110, 13, 0xf5e49c);
    
    banderaPantalla = 1; // seteamos banderaPantalla para entrar a la eleccion de juego
    banderaUnJugador = 0;
    banderaDosJugador = 0;
    banderaMartillito = 0;
    banderaInGame = 0;
    Serial.println("Menu completado");
  }

//***************************************************************************************************************************************
// MENU
//***************************************************************************************************************************************

  while (banderaPantalla == 1 & banderaInGame == 0)  
  {
    inByte = 0; // quitamos valor de inByte
    if (Serial5.available()) 
    {
      inByte = Serial5.read();
      Serial.println(inByte);
      
// ---------------------------------------------------------------------
// Subir de opcion 
// ---------------------------------------------------------------------
      if (inByte == 10 & banderaInGame == 0) { 
        LCD_Bitmap(88, 128, 8, 8, martillito);
        FillRect(88, 144, 8, 8, 0x00);
        Rect(108, 125, 110, 13, 0xf5e49c);
        Rect(108, 140, 110, 14, 0x00);
        banderaMartillito = 0;
        Serial.println("En opcion: 1 player game");
      }

// ---------------------------------------------------------------------
// Bajar de opcion 
// ---------------------------------------------------------------------
      else if (inByte == 20 & banderaInGame == 0) {
        LCD_Bitmap(88, 144, 8, 8, martillito);
        FillRect(88, 128, 8, 8, 0x00);
        Rect(108, 140, 110, 14, 0xf5e49c);
        Rect(108, 125, 110, 13, 0x00);
        banderaMartillito = 1;
        Serial.println("En opcion: 2 player gamer");
      }

// ---------------------------------------------------------------------
// Seleccionar juego en 1 jugador 
// ---------------------------------------------------------------------

      if (inByte == 50 & banderaMartillito == 0 & banderaInGame == 0) {
        Serial.println("Juego en 1 jugador");
        LCD_Clear(0x00);
        banderaUnJugador = 1;
        banderaDosJugador = 0;
        banderaPantalla = 2;
        inByte = 0;
      }

// ---------------------------------------------------------------------
// Seleccionar juego en 2 jugadores 
// ---------------------------------------------------------------------
      if (inByte == 50 & banderaMartillito == 1 & banderaInGame == 0) {
        Serial.println("Juego en 2 jugadores");
        LCD_Clear(0x00);
        banderaUnJugador = 0;
        banderaDosJugador = 1;
        banderaPantalla = 2;
        inByte = 0;
      }
    }
  }

//***************************************************************************************************************************************
// JUEGOOOO
//***************************************************************************************************************************************

  while (banderaPantalla == 2  & banderaInGame == 0) // 1ra pantalla de juego
  {
    inByte = 0;
    myFile = SD.open("prueba2.txt"); // desplegamos la primera pantalla de juego
    mapSD();
    banderaInGame = 1; // setemaos que estamos en un juego
    banderaPantalla = 3;  // cambiamos al juego
    Serial.println(banderaUnJugador);
    Serial.println(banderaDosJugador);
    Serial.println(banderaInGame);
    Serial.println(banderaPantalla);
  }


  while (banderaPantalla == 3) // Pantalla de juego
  {
    if (inByte == 50) // se sale del juego y regresa al menu
    { 
      LCD_Clear(0x00);
      banderaPantalla = 0;
      banderaInGame = 0;
    }

// ---------------------------------------------------------------------
// Despligue de enemigos 
// ---------------------------------------------------------------------

    for (int i = 0; i < 1; i++)
    {
      LCD_Sprite(enemigo_x[0], enemigo_y[0], Bird_width, Bird_height, Bird, 2, 1, 0, 1);
    }

    for (int i = 0; i < 1; i++)
    {
      LCD_Sprite(70, 152, iceman_width, iceman_height, iceman, 3, 1, 0, 1);
    }
    
// ---------------------------------------------------------------------
// Movimiento de enemigos 
// ---------------------------------------------------------------------
    if (enemies_move)
    {
      for (int i = 0; i < 1; i++) {
        delay(50);
        FillRect(enemigo_x[0] + Bird_width, enemigo_y[0], 2, 20, 0x000);
        enemigo_x[0] -= 2;
      }
      if (enemigo_x[0] <= map_border_left + 10)
      {
        enemies_move = false;
      }
    } else
    {
      for (int i = 0; i < 1; i++)
      {
        delay(50);
        FillRect(enemigo_x[0] - 2, enemigo_y[i], 2, 20, 0x000);
        enemigo_x[0] += 2;
      }
      if (enemigo_x[0] >= map_border_right - 12) {
        enemies_move = true;
      }
    }

// ---------------------------------------------------------------------
// Desplegar bloques obstaculo 
// ---------------------------------------------------------------------
    while (block_position_x <= 248)
    {
      for (int i = 0; i < 4; i++)
      {
        if (block_position_y[i] > 120)
        {
          LCD_Bitmap(block_position_x, block_position_y[i], 8, 8, block_green);

          int x_aleatorio = ((std::rand() % 24) + 64) * 8 + 64;
          LCD_Bitmap(x_aleatorio, block_random_y[i], 8, 4, rect_green);
        } else {
          LCD_Bitmap(block_position_x, block_position_y[i], 8, 4, block_brown);

          int x_aleatorio = ((std::rand() % 24) + 64) * 8 + 64;
          LCD_Bitmap(x_aleatorio, block_random_y[i] - 3, 8, 4, rect_brown);
        }
      }
      block_position_x += 8;
    }

// ---------------------------------------------------------------------
// PERSONAJES 
// ---------------------------------------------------------------------
    inByte = 0;
    if (Serial5.available())
    {
      inByte = Serial5.read();

// ---------------------------------------------------------------------
// UN JUGADOR
// ---------------------------------------------------------------------
      if (banderaUnJugador == 1 & banderaInGame == 1 & banderaDosJugador == 0)
      {
        Serial.println(IceClimber1_x);
        LCD_Bitmap(IceClimber1_x, IceClimber1_y, IceClimber1_width, IceClimber1_height, iceclimber1);
        // MUEVE A LA IZQUIERDA
        if (inByte == 40)
        {
          LimiteIzquierdaIC_1 = false;
          if (LimiteDerechaIC_1 == false)
          {
            IceClimber1_x += 4;
            if (IceClimber1_x >= 230)
            {
              LimiteDerechaIC_1 = true;
            }
          }
          FillRect(IceClimber1_x - 4, IceClimber1_y, 4, IceClimber1_height, 0x000);
          Serial.println(inByte);
        }

        // MUEVE A LA DERECHA
        if (inByte == 30)
        {
          LimiteDerechaIC_1 = false;
          if (LimiteIzquierdaIC_1 == false)
          {
            IceClimber1_x -= 4;
            if (IceClimber1_x <= 73)
            {
              LimiteIzquierdaIC_1 = true;
            }
          }
          FillRect(IceClimber1_x + IceClimber1_width, IceClimber1_y, 3, IceClimber1_height, 0x000);
          Serial.println(inByte);
        }
      }

// ---------------------------------------------------------------------
// DOS JUGADORES
// ---------------------------------------------------------------------
      if (banderaDosJugador == 1 & banderaInGame == 1 & banderaUnJugador == 0)
      {
        LCD_Bitmap(IceClimber1_x, IceClimber1_y, IceClimber1_width, IceClimber1_height, iceclimber1);
        LCD_Bitmap(IceClimber2_x, IceClimber2_y, IceClimber2_width, IceClimber2_height, iceclimber2);

        if (inByte == 40)
        {
          LimiteIzquierdaIC_1 = false;
          if (LimiteDerechaIC_1 == false)
          {
            IceClimber1_x += 4;
            if (IceClimber1_x >= 230)
            {
              LimiteDerechaIC_1 = true;
            }
          }
          FillRect(IceClimber1_x - 4, IceClimber1_y, 4, IceClimber1_height, 0x000);
        }

        if (inByte == 30)
        {
          LimiteDerechaIC_1 = false;
          if (LimiteIzquierdaIC_1 == false)
          {
            IceClimber1_x -= 4;
            if (IceClimber1_x <= 73)
            {
              LimiteIzquierdaIC_1 = true;
            }
          }
          FillRect(IceClimber1_x + IceClimber1_width, IceClimber1_y, 3, IceClimber1_height, 0x000);
        }

        if (inByte == 10)
        {
          LimiteIzquierdaIC_2 = false;
          if (LimiteDerechaIC_2 == false)
          {
            IceClimber2_x += 4;
            if (IceClimber2_x >= 230)
            {
              LimiteDerechaIC_2 = true;
            }
          }
          FillRect(IceClimber2_x - 4, IceClimber2_y, 4, IceClimber2_height, 0x000);
        }

        if (inByte == 20)
        {
          LimiteDerechaIC_2 = false;
          if (LimiteIzquierdaIC_2 == false)
          {
            IceClimber2_x -= 4;
            if (IceClimber2_x <= 73)
            {
              LimiteIzquierdaIC_2 = true;
            }
          }
          FillRect(IceClimber2_x + IceClimber2_width, IceClimber2_y, 3, IceClimber2_height, 0x000);
        }

      }
    }
  }

}

















//***************************************************************************************************************************************
// Funciones para proyecto
//***************************************************************************************************************************************

bool Collision (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
  return (x1 < x2 + w2) && (x1 + w1 > x2) && (y1 < y2 + h2) && (y1 + h1 > y2);
}

void mapSD(void) {
  int hex1 = 0;
  int val1 = 0;
  int val2 = 0;
  int mapar = 0;
  int vert = 0;
  if (myFile)
  {
    Serial.println("Leyendo archivo...");
    while (myFile.available())
    {
      mapar = 0;
      while (mapar < 640)
      {
        hex1 = myFile.read();
        if (hex1 == 120)
        {
          val1 = myFile.read();
          val2 = myFile.read();
          val1 = ASCII_Hex(val1);
          val2 = ASCII_Hex(val2);
          maps[mapar] = val1 * 16 + val2;
          mapar++;
        }
      }
      LCD_Bitmap(0, vert, 320, 1, maps);
      vert++;
    }
    myFile.close();
  }
}

int ASCII_Hex(int a) {
  switch (a) {
    case 48:
      return 0;
    case 49:
      return 1;
    case 50:
      return 2;
    case 51:
      return 3;
    case 52:
      return 4;
    case 53:
      return 5;
    case 54:
      return 6;
    case 55:
      return 7;
    case 56:
      return 8;
    case 57:
      return 9;
    case 97:
      return 10;
    case 98:
      return 11;
    case 99:
      return 12;
    case 100:
      return 13;
    case 101:
      return 14;
    case 102:
      return 15;
  }
}


//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
  }
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + w;
  y2 = y + h;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = w * h * 2 - 1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);

      //LCD_DATA(bitmap[k]);
      k = k - 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}
