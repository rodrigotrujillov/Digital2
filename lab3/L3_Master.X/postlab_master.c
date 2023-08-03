
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
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

//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#include "SPI.h"
#include "LCD8bits.h"

//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
#define RS RE0
#define EN RE1

uint8_t z;
float pot_slave1;
float pot_slave2;
uint8_t cont;
float voltaje_s1, voltaje_s2;
char centenas, decenas, unidad;
char val_s1[20], val_s2[20];
char valor[20];
//*****************************************************************************
// Prototipos de funciones
//*****************************************************************************
void setup(void);

float map(float x, float in_min, float in_max, float out_min, float out_max);

//*****************************************************************************
// Código Principal
//*****************************************************************************
void main(void){
    setup();
    Lcd_Init();
    
    while(1)
    {
        PORTCbits.RC6 = 0;  // Slave 1 select
        __delay_ms(1);
        
        spiWrite(z);
        pot_slave1 = spiRead();

        __delay_ms(1);
        PORTCbits.RC6 = 1;  // Slave 1 deselect
        
        __delay_ms(50);
//**********************************************************************
        PORTCbits.RC7 = 0; // Slave 2 select
        __delay_ms(1);
        
        spiWrite(0);     // mandamos 0x00 al buffer
        cont = spiRead();
        
        __delay_ms(1);
        PORTCbits.RC7 = 1; // Slave 2 deselect
        
        __delay_ms(50); 
//**********************************************************************
        PORTCbits.RC7 = 0; // Slave 2 select
        __delay_ms(1);
        
        spiWrite(1);         // mandamos 0x01 al buffer
        pot_slave2 = spiRead();
        
        __delay_ms(1);
        PORTCbits.RC7 = 1; // Slave 2 deselect
        
        __delay_ms(50);
//**********************************************************************
        
        voltaje_s1 = map(pot_slave1, 0, 255, 0, 5);
        sprintf(val_s1, "%.2f", voltaje_s1);
        
        voltaje_s2 = map(pot_slave2, 0, 255, 0, 5);
        sprintf(val_s2, "%.2f", voltaje_s2);
        
        centenas =  (cont/100);
        decenas = (cont/10)%10;
        unidad = cont%10;
        
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("S1:");
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(val_s1);
        
        Lcd_Set_Cursor(1,7);
        Lcd_Write_String("S2:");
        Lcd_Set_Cursor(2,7);
        Lcd_Write_String(val_s2);
       
        Lcd_Set_Cursor(1,13);
        Lcd_Write_String("S3:");
        Lcd_Set_Cursor(2,13);
        Lcd_Write_Char(centenas + 48);
        Lcd_Write_Char(decenas + 48);
        Lcd_Write_Char(unidad + 48);
    }
    return;
}

//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){    
    ANSEL = 0;
    ANSELH = 0;
    
    TRISC6 = 0;
    TRISC7 = 0;
    
    TRISB = 0;
    PORTB = 0;
    
    TRISD = 0;
    PORTD = 0;
    
    TRISE = 0;
    PORTE = 0;
    
    PORTCbits.RC6 = 1;
    PORTCbits.RC7 = 1;
    
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

