/*
 * File:   Proyecto1_Master.c
 * Author: rodri
 *
 * Created on 17 de agosto de 2023, 11:36 AM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//LIBRERÍAS
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>


#include "SPI.h"
#include "i2c.h"
#include "LCDi2c.h"


//CONSTANTES
#define _XTAL_FREQ 8000000


//VARIABLES
uint8_t pot1;
uint8_t pot2;
uint8_t dist;
int led;
int motor;
float temp;
float voltaje2;
unsigned char valor[20];
unsigned char valor2[20];

char centenas;
char decenas;
char unidad;
char selection;
int a;
int b;

//PROTOTIPO DE FUNCIONES
void setup(void);

//MAIN
void main(void) {
    setup();
    
    I2C_Init_Master(I2C_100KHZ);        // Inicializa el protocolo i2c
    Lcd_Init();                         // Inicializa la pantalla LCD
    
    while(1){
        
       
       I2C_Start();
       I2C_Write(0x71);
       dist = I2C_Read();
       I2C_Stop();
       __delay_ms(100); 
       
       if (dist < 20){
           motor = 1;
       }
       else{
           motor = 0;
       }
        
       I2C_Start();
       I2C_Write(0x51);
       pot1 = I2C_Read();
       I2C_Stop();
       __delay_ms(100);
       
      temp = ((pot1*(float)150)/(float)77); // Conversión del valor del pot del slave 1 a valor de voltaje
       
       
       I2C_Start();
       I2C_Write(0x61);
       pot2 = I2C_Read();
       I2C_Stop();
       __delay_ms(100);
       
       if (pot2 >= 100){
           led = 1;
       }
       else{
           led = 0;
       }
       
       
       sprintf(valor, "%0.2f", temp);
        
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Temp");
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(valor);
        Lcd_Set_Cursor(2, 6);
        Lcd_Write_Char(223);
        Lcd_Write_String("C");
        
        
        /*__delay_ms(1000);
        Lcd_Clear();
        __delay_ms(500);*/
 
        if (led == 0){
            Lcd_Set_Cursor(1,9);
            Lcd_Write_String("LUZ");
            Lcd_Set_Cursor(2,9);
            Lcd_Write_String("OFF");
        }
        else if (led == 1){
            Lcd_Set_Cursor(1,9);
            Lcd_Write_String("LUZ");
            Lcd_Set_Cursor(2,9);
            Lcd_Write_String("ON ");
        }
        
        if (motor == 0){
            Lcd_Set_Cursor(1,13);
            Lcd_Write_String("Dist");
            Lcd_Set_Cursor(2,13);
            Lcd_Write_String("OFF");
        }
        else if (motor == 1){
            Lcd_Set_Cursor(1,13);
            Lcd_Write_String("Dist");
            Lcd_Set_Cursor(2,13);
            Lcd_Write_String("ON ");
        }
        
        /*__delay_ms(1000);
        Lcd_Clear();
        __delay_ms(500);*/
        
        
        
      
    }
    return;
}

//CONFIGURACIONES
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    PORTA = 0;
    
    TRISE = 0;
    PORTE = 0;
    
    TRISB = 0;
    TRISD = 0;
    PORTB = 0;
    PORTD = 0;
    
    // Configuración del LCD
    //__delay_ms(500);
  


}
