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
//
// Functions Prototypes
//
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

int inByte;

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

// DATOS DE MAPA ---------------------------------------------------------------
int map_border_left = 73; // borde mapa izquierda
int map_border_right = 230; // borde mapa derecha
int map_floor = 190; // borde del piso
int map_floor2 = 190;


bool LimiteDerechaIC_1 = false; // limite izquierda ice climber 1
bool LimiteIzquierdaIC_1 = false; // limite izquierda ice climber 2
bool LimiteDerechaIC_2 = false; // limite izquierda ice climber 1
bool LimiteIzquierdaIC_2 = false; // limite izquierda ice climber 2

// DATOS DE JUGADORES -----------------------------------------------------------
int IceClimber1_x = 145; // posicion inicial en X de ice climber 1
int IceClimber1_y = 190; // posicion inicial en Y de ice climber 1
int IceClimber1_width = 16; // ancho ice climber 1
int IceClimber1_height = 24; // altura ice climber 1

int IceClimber2_x = 225; // posicion inicial en X de ice climber 2
int IceClimber2_y = 190; // posiicion inicial en Y de ice climber 2
int IceClimber2_width = 16; // ancho ice climber 2
int IceClimber2_height = 24; // altura ice climber 2

int jumpHeight; // altura de salto;
int jumpHeight2;


// BANDERAS DE ESTADOS ----------------------------------------------------------
int banderaPantalla = 0;
int banderaMartillito = 0;
int modojuego = 0;
bool unJugador = false;
bool dosJugador = false;
bool inGame = false;

bool onAir = false;
bool onGround = false;
bool jumping = false;
bool isFloor = false;
bool walkingRight = false;
bool walkingLeft = false;
bool colision = false;

bool onAir2 = false;
bool onGround2 = false;
bool jumping2 = false;
bool isFloor2 = false;
bool walkingRight2 = false;
bool walkingLeft2 = false;


// BLOQUES ---------------------------------------------------------------------
int block_position_x = 64;
int block_position_y[5] = {216, 168, 120, 72, 24};
int block_random_y[4] = {176, 128, 80, 32};
int blocks = 12;

int blockx = 200;
int blocky = 176;
int block_w = 8;
int block_h = 8;

int set_blocks_green[24];
int set_slabes_green[24];
int set_blocks_brown[24];
int set_blocks_x_index = 0;


//***************************************************************************************************************************************
//
// Inicialización
//
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
}
//***************************************************************************************************************************************
//
// Loop Infinito
//
//***************************************************************************************************************************************
void loop() {

  while (banderaPantalla == 0 & !inGame) {
    LCD_Clear(0x00);
    Serial.println("Iniciando Juego");
    myFile = SD.open("menu.txt"); // extraemos la imagen del menu de la SD
    mapSD();
    LCD_Bitmap(88, 128, 8, 8, martillito); // colocamos el martillito en opcion 1 Jugador
    FillRect(88, 144, 8, 8, 0x00);
    Rect(108, 125, 110, 13, 0xf5e49c);

    banderaPantalla = 1;
    banderaMartillito = 0;
    unJugador = false;
    dosJugador = false;
    inGame = false;
  }

  //***************************************************************************************************************************************
  //
  // MENU
  //
  //***************************************************************************************************************************************

  while (banderaPantalla == 1 & !inGame)
  {
    inByte = 0; // quitamos valor de inByte
    if (Serial5.available())
    {
      inByte = Serial5.read();
      Serial.println(inByte);

      // ---------------------------------------------------------------------
      // Subir de opcion
      // ---------------------------------------------------------------------
      if (inByte == 10 & !inGame) {
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
      else if (inByte == 20 & !inGame) {
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

      if (inByte == 50 & banderaMartillito == 0 & !inGame) {
        Serial.println("Juego en 1 jugador");
        LCD_Clear(0x00);
        modojuego = 1;
        banderaPantalla = 2;
        inByte = 0;
      }

      // ---------------------------------------------------------------------
      // Seleccionar juego en 2 jugadores
      // ---------------------------------------------------------------------
      if (inByte == 50 & banderaMartillito == 1 & !inGame) {
        Serial.println("Juego en 2 jugadores");
        LCD_Clear(0x00);
        modojuego = 2;
        banderaPantalla = 2;
        inByte = 0;
      }
    }
  }

  //***************************************************************************************************************************************
  //
  // JUEGOOOO
  //
  //***************************************************************************************************************************************

  while (banderaPantalla == 2 & !inGame) {
    inByte = 0;
    myFile = SD.open("prueba2.txt"); // desplegamos la primera pantalla de juego
    mapSD();
    inGame = true; // setemaos que estamos en un juego
    banderaPantalla = 3;  // cambiamos al juego
    Serial.println(banderaPantalla);
    Serial.println(inGame);
    Serial.println(modojuego);
  }

  while (block_position_x <= 248)
  {
    for (int i = 0; i < 5; i++)
    {
      if (block_position_y[i] > 120)
      {
        LCD_Bitmap(block_position_x, block_position_y[i], 8, 8, block_green);
        //int x_aleatorio = ((std::rand() % 24) + 64) * 8 + 64;
        //LCD_Bitmap(x_aleatorio, block_random_y[i], 8, 4, rect_green);

        set_blocks_green[set_blocks_x_index] = block_position_x;
        // set_slabes_green[set_blocks_x_index] = x_aleatorio;
        set_blocks_x_index++;

      } else {
        LCD_Bitmap(block_position_x, block_position_y[i], 8, 8, block_brown);
        //int x_aleatorio = ((std::rand() % 24) + 64) * 8 + 64;
        //LCD_Bitmap(x_aleatorio, block_random_y[i] - 3, 8, 4, rect_brown);
      }
    }
    block_position_x += 8;
  }

  //***************************************************************************************************************************************
  //
  // UN JUGADOR
  //
  //***************************************************************************************************************************************
  while (banderaPantalla == 3 & inGame & modojuego == 1) {

    LCD_Bitmap(IceClimber1_x, IceClimber1_y, IceClimber1_width, IceClimber1_height, iceclimber1);
    inByte = 0;
    if (Serial5.available())
    {
      inByte = Serial5.read();
      Serial.println(inByte);
      if (inByte == 50 && !jumping) {
        jumping = true;
        jumpHeight = IceClimber1_y - 62;
      }
      if (inByte == 30 && !walkingRight) {
        walkingRight = true;
      }
      if (inByte == 40 && !walkingLeft) {
        walkingLeft = true;
      }
    }

    bool onFloor = false;
    for (int i = 0; i < 23; i++) {
      if (IceClimber1_y + IceClimber1_height >= block_position_y[1] &&
          IceClimber1_y + IceClimber1_height <= block_position_y[1] + 8 &&
          IceClimber1_x + IceClimber1_width >= set_blocks_green[i] &&
          IceClimber1_x <= set_blocks_green[i] + 8) {
        map_floor = IceClimber1_y;
        onFloor = true;
      } else if (Collision(IceClimber1_x, IceClimber1_y, IceClimber1_width, IceClimber1_height, set_blocks_green[i], block_position_y[1], 8, 8)) {
        set_blocks_green[i] = 0;
        onAir = true;
      }
    }

    // ---------------------------------------------------------------------
    // Saltar
    // ---------------------------------------------------------------------
    if (jumping) {
      if (onAir) {
        if (IceClimber1_y < map_floor) {
          IceClimber1_y++;
          delay(10);
        } else {
          onAir = false;
          jumping = false;
        }
      } else {
        if (IceClimber1_y > jumpHeight) {
          IceClimber1_y--;
          delay(10);
        } else {
          onAir = true;
        }
      }
      FillRect(IceClimber1_x, IceClimber1_y + IceClimber1_height, IceClimber1_width, 1, 0x000);
      FillRect(IceClimber1_x, IceClimber1_y, IceClimber1_width, 1, 0x00);
    }


    // ---------------------------------------------------------------------
    // Caminar derecha
    // ---------------------------------------------------------------------
    if (walkingRight) {
      LimiteDerechaIC_1 = false;
      if (!LimiteIzquierdaIC_1) {
        IceClimber1_x -= 4;
        walkingRight = false;
        if (IceClimber1_x <= 73) {
          LimiteIzquierdaIC_1 = true;
          walkingRight = false;
        }
      }
      FillRect(IceClimber1_x + IceClimber1_width, IceClimber1_y, 3, IceClimber1_height, 0x000);
    }

    // ---------------------------------------------------------------------
    // Caminar izquierda
    // ---------------------------------------------------------------------
    if (walkingLeft) {
      LimiteIzquierdaIC_1 = false;
      if (LimiteDerechaIC_1 == false) {
        IceClimber1_x += 4;
        walkingLeft = false;
        if (IceClimber1_x >= 230) {
          LimiteDerechaIC_1 = true;
          walkingLeft = false;
        }
      }
      FillRect(IceClimber1_x - 4, IceClimber1_y, 4, IceClimber1_height, 0x000);
    }
  }

  //***************************************************************************************************************************************
  //
  // DOS JUGADORES
  //
  //***************************************************************************************************************************************

  while (banderaPantalla == 3 & inGame & modojuego == 2) {
    LCD_Bitmap(IceClimber1_x, IceClimber1_y, IceClimber1_width, IceClimber1_height, iceclimber1);
    LCD_Bitmap(IceClimber2_x, IceClimber2_y, IceClimber2_width, IceClimber2_height, iceclimber2);
    inByte = 0;

    if (Serial5.available())
    {
      inByte = Serial5.read();
      Serial.println(inByte);
      if (inByte == 50 && !jumping) {
        jumping = true;
        jumpHeight = IceClimber1_y - 62;
      }
      if (inByte == 60 && !jumping2) {
        jumping2 = true;
        jumpHeight2 = IceClimber2_y - 62;
      }
      if (inByte == 30 && !walkingRight) {
        walkingRight = true;
      }
      if (inByte == 40 && !walkingLeft) {
        walkingLeft = true;
      }
      if (inByte == 10 && !walkingLeft2) {
        walkingLeft2 = true;
      }
      if (inByte == 20 && !walkingRight2) {
        walkingRight2 = true;
      }
    }

    bool onFloor = false;
    bool onFloor2 = false;
    for (int i = 0; i < 23; i++) {
      if (IceClimber1_y + IceClimber1_height >= block_position_y[1] &&
          IceClimber1_y + IceClimber1_height <= block_position_y[1] + 8 &&
          IceClimber1_x + IceClimber1_width >= set_blocks_green[i] &&
          IceClimber1_x <= set_blocks_green[i] + 8) {
        map_floor = IceClimber1_y;
        onFloor = true;
      } else if (Collision(IceClimber1_x, IceClimber1_y, IceClimber1_width, IceClimber1_height, set_blocks_green[i], block_position_y[1], 8, 8)) {
        set_blocks_green[i] = 0;
        onAir = true;
      }
    }

    for (int i = 0; i < 23; i++) {
      if (IceClimber2_y + IceClimber2_height >= block_position_y[1] &&
          IceClimber2_y + IceClimber2_height <= block_position_y[1] + 8 &&
          IceClimber2_x + IceClimber2_width >= set_blocks_green[i] &&
          IceClimber2_x <= set_blocks_green[i] + 8) {
        map_floor2 = IceClimber2_y;
        onFloor2 = true;
      } else if (Collision(IceClimber2_x, IceClimber2_y, IceClimber2_width, IceClimber2_height, set_blocks_green[i], block_position_y[1], 8, 8)) {
        set_blocks_green[i] = 0;
        onAir2 = true;
      }
    }


    // ---------------------------------------------------------------------
    // Saltar 1
    // ---------------------------------------------------------------------
    if (jumping) {
      if (onAir) {
        if (IceClimber1_y < map_floor) {
          IceClimber1_y++;
          delay(10);
        } else {
          onAir = false;
          jumping = false;
        }
      } else {
        if (IceClimber1_y > jumpHeight) {
          IceClimber1_y--;
          delay(10);
        } else {
          onAir = true;
        }
      }
      FillRect(IceClimber1_x, IceClimber1_y + IceClimber1_height, IceClimber1_width, 1, 0x000);
      FillRect(IceClimber1_x, IceClimber1_y, IceClimber1_width, 1, 0x00);
    }

    // ---------------------------------------------------------------------
    // Saltar 2
    // ---------------------------------------------------------------------
    if (jumping2) {
      if (onAir2) {
        if (IceClimber2_y < map_floor2) {
          IceClimber2_y++;
          delay(10);
        } else {
          onAir2 = false;
          jumping2 = false;
        }
      } else {
        if (IceClimber2_y > jumpHeight2) {
          IceClimber2_y--;
          delay(10);
        } else {
          onAir2 = true;
        }
      }
      FillRect(IceClimber2_x, IceClimber2_y + IceClimber2_height, IceClimber2_width, 1, 0x000);
      FillRect(IceClimber2_x, IceClimber2_y, IceClimber2_width, 1, 0x00);
    }

    // ---------------------------------------------------------------------
    // caminar derecha 1
    // ---------------------------------------------------------------------
    if (walkingRight) {
      LimiteDerechaIC_1 = false;
      if (!LimiteIzquierdaIC_1) {
        IceClimber1_x -= 4;
        walkingRight = false;
        if (IceClimber1_x <= 73) {
          LimiteIzquierdaIC_1 = true;
          walkingRight = false;
        }
      }
      FillRect(IceClimber1_x + IceClimber1_width, IceClimber1_y, 3, IceClimber1_height, 0x000);
    }
    
    //----------------------------------------------------------------------
    // caminar derecha 2
    // ---------------------------------------------------------------------
    if (walkingRight2) {
      LimiteDerechaIC_2 = false;
      if (!LimiteIzquierdaIC_2) {
        IceClimber2_x -= 4;
        walkingRight2 = false;
        if (IceClimber2_x <= 73) {
          LimiteIzquierdaIC_2 = true;
          walkingRight2 = false;
        }
      }
      FillRect(IceClimber2_x + IceClimber2_width, IceClimber2_y, 3, IceClimber2_height, 0x000);
    }

    // ---------------------------------------------------------------------
    // caminar izquierda 1
    // ---------------------------------------------------------------------
    if (walkingLeft) {
      LimiteIzquierdaIC_1 = false;
      if (LimiteDerechaIC_1 == false) {
        IceClimber1_x += 4;
        walkingLeft = false;
        if (IceClimber1_x >= 230) {
          LimiteDerechaIC_1 = true;
          walkingLeft = false;
        }
      }
      FillRect(IceClimber1_x - 4, IceClimber1_y, 4, IceClimber1_height, 0x000);
    }

    // ---------------------------------------------------------------------
    // caminar izquierda 2
    // ---------------------------------------------------------------------
    if (walkingLeft2) {
      LimiteIzquierdaIC_2 = false;
      if (LimiteDerechaIC_2 == false) {
        IceClimber2_x += 4;
        walkingLeft2 = false;
        if (IceClimber2_x >= 230) {
          LimiteDerechaIC_2 = true;
          walkingLeft2 = false;
        }
      }
      FillRect(IceClimber2_x - 4, IceClimber2_y, 4, IceClimber2_height, 0x000);
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
