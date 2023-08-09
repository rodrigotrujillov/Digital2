// 
// POSTLAB MASTER
// 
//*****************************************************************************
// Palabra de configuraci?n
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// Definicion de librerias
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <pic16f887.h>
#include "I2C.h"
#include <xc.h>
#include "LCD8bits.h"
#include "iocb.h"
//*****************************************************************************
// Definicion de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
int sec = 00;
int min = 00;
int hora = 00;
int dia = 1;
int mes = 1;
int year = 19;
float pot;
float temperatura;
char valor[20];
//*****************************************************************************
// Definicion de prototipos
//*****************************************************************************
void setup(void);
int BCD_A_DEC(int convertir);
int DEC_A_BCD(int convertir);
void setear_dia_tiempo();
void update_dia_tiempo();

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    while(1){
        // RCT
        char dia_0 = dia % 10;
        char dia_1 = dia / 10;
        char mes_0 = mes % 10;
        char mes_1 = mes / 10;
        char year_0 = year % 10;
        char year_1 = year / 10;
        
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Date: ");
        Lcd_Write_Char(dia_1 + '0');
        Lcd_Write_Char(dia_0 + '0');
        Lcd_Write_String("/");
        Lcd_Write_Char(mes_1 + '0');
        Lcd_Write_Char(mes_0 + '0');
        Lcd_Write_String("/");
        Lcd_Write_Char(year_1 + '0');
        Lcd_Write_Char(year_0 + '0');
        
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String("Temp: ");
        sprintf(valor, "%.2f", pot);
        Lcd_Write_String(valor);
        Lcd_Set_Cursor(2,14);
        Lcd_Write_String(" C");
        
        if (PORTBbits.RB0 == 0){
            while (!PORTBbits.RB0);
            dia++;
            if (dia > 31) dia = 0;
            I2C_Master_Start(); 
            I2C_Master_Write(0xD0);        
            I2C_Master_Write(4);  
            I2C_Master_Write(DEC_A_BCD(dia));        
            I2C_Master_Stop();  
        }
        
        if (PORTBbits.RB1 == 0){
            while (!PORTBbits.RB1);
            mes++;
            if (mes > 12) mes = 1;
            I2C_Master_Start(); 
            I2C_Master_Write(0xD0);        
            I2C_Master_Write(5);  
            I2C_Master_Write(DEC_A_BCD(mes));        
            I2C_Master_Stop();  
        }
        
        if (PORTBbits.RB2 == 0){
            while (!PORTBbits.RB2);
            year++;
            if (year > 30) year = 0;
            I2C_Master_Start(); 
            I2C_Master_Write(0xD0);        
            I2C_Master_Write(6);  
            I2C_Master_Write(DEC_A_BCD(year));        
            I2C_Master_Stop();  
        }
        
        
        // SLAVE
        
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        pot = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(200); 

        
    }
    return;
}
//*****************************************************************************
// Funciones
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    PORTA = 0;
    TRISB = 0b00000111;
    TRISD = 0;
    PORTB = 0;
    PORTD = 0;
    TRISE = 0;
    PORTE = 0;
    ioc_init(0);
    ioc_init(1);
    ioc_init(2);
    Lcd_Init();
    I2C_Master_Init(100000);
    setear_dia_tiempo();
}

int BCD_A_DEC(int convertir){
    return (convertir >> 4) * 10 + (convertir & 0x0F);
}
int DEC_A_BCD(int convertir){
    return ((convertir / 10) << 4) + (convertir % 10);
}

void setear_dia_tiempo(){
    I2C_Master_Start();            
    I2C_Master_Write(0xD0);        
    I2C_Master_Write(0x00);        
    I2C_Master_Write(DEC_A_BCD(sec));         
    I2C_Master_Write(DEC_A_BCD(min));         
    I2C_Master_Write(DEC_A_BCD(hora));        
    I2C_Master_Write(0x01);        
    I2C_Master_Write(DEC_A_BCD(dia));         
    I2C_Master_Write(DEC_A_BCD(mes));         
    I2C_Master_Write(DEC_A_BCD(year));        
    I2C_Master_Stop();             
}

void update_dia_tiempo(){
    // Empezamos a leer
    I2C_Master_Start();
    I2C_Master_Write(0xD0);
    I2C_Master_Write(0);
    I2C_Master_Stop();
    
    // Leemos
    I2C_Master_Start();
    I2C_Master_Write(0xD1);
    sec = BCD_A_DEC(I2C_Master_Read(1));
    min = BCD_A_DEC(I2C_Master_Read(1));
    hora = BCD_A_DEC(I2C_Master_Read(1));
    I2C_Master_Read(1);
    dia = BCD_A_DEC(I2C_Master_Read(4));
    mes = BCD_A_DEC(I2C_Master_Read(5));
    year = BCD_A_DEC(I2C_Master_Read(6));
    I2C_Master_Stop();
    
    // Terminamos de leer
    I2C_Master_Start();
    I2C_Master_Write(0xD1);
    I2C_Master_Read(1);
    I2C_Master_Stop();
    
}

