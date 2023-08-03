
//*****************************************************************************
// Palabra de configuraci�n
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
// Definici�n e importaci�n de librer�as
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#include "SPI.h"

//*****************************************************************************
// Definici�n de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000

uint16_t puertob1;
uint16_t puertod1;

uint16_t puertob2;
uint16_t puertod2;

//*****************************************************************************
// Prototipos de funciones
//*****************************************************************************
void setup(void);

//*****************************************************************************
// C�digo Principal
//*****************************************************************************
void main(void){
    setup();
    
    while(1)
    {
        PORTCbits.RC2 = 0;  // Slave 1 select
        __delay_ms(1);
        
        spiWrite(puertob1);
        puertod1 = spiRead();
        
        __delay_ms(1);
        PORTCbits.RC2 = 1;  // Slave 1 deselect
        
        __delay_ms(100);
        
        PORTCbits.RC1 = 0; // Slave 2 select
        __delay_ms(1);
        
        spiWrite(puertob2);
        puertod2 = spiRead();
        
        __delay_ms(1);
        PORTCbits.RC1 = 1;
        
        PORTB = puertod1;
        PORTD = puertod2;
        
        __delay_ms(250);
    }
    return;
}

//*****************************************************************************
// Funci�n de Inicializaci�n
//*****************************************************************************
void setup(void){    
    ANSEL = 0;
    ANSELH = 0;
    
    TRISC2 = 0;
    TRISC1 = 0;
    TRISD = 0;
    TRISB = 0;
    PORTB = 0;
    PORTD = 0;
    PORTCbits.RC2 = 1;
    PORTCbits.RC1 = 1;
    
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}