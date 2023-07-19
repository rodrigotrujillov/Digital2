/*
 * Archivo:   prelab.c
 * Dispositivo: PIC16F887
 * Compilador: XC8
 * Autor: Rodrigo Trujillo
 * Programa: Contador de 8 bits utilizando 2 botones y desplegados en un puerto
 * Hardware: Botones en PORTB, LEDs en PORTD
 * 
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
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

#include <xc.h>
#include <stdint.h>

//******************************************************************************
// Constantes y variables
//******************************************************************************

#define _XTAL_FREQ 8000000
#define aumentar PORTBbits.RB0
#define disminuir PORTBbits.RB1

//******************************************************************************
// Prototipos
//******************************************************************************

void setup(void);

//******************************************************************************
// Interrupciones
//******************************************************************************

void __interrupt() isr(void) {
    
    // interrupcion RB para contador
    if (INTCONbits.RBIF) {
        if (!aumentar) {
            PORTC++;
        }
        if (!disminuir) {
            PORTC--;
        }
        INTCONbits.RBIF = 0;
    }
}

//******************************************************************************
// MAIN
//******************************************************************************

void main(void) {
    setup();
    while(1){
    }
    return;
}

//******************************************************************************
// Setup
//******************************************************************************

void setup(void){
    // Configuracion de puertos
    ANSEL = 0;
    ANSELH = 0;
    
    TRISB = 0b00000011;
    PORTB = 0;
    
    TRISC = 0;
    PORTC = 0;
    
    // Configuracion oscilador
    OSCCONbits.IRCF = 0b111;
    OSCCONbits.SCS = 1;
    
    // Configuracion WPUB
    OPTION_REGbits.nRBPU = 0;
    WPUBbits.WPUB0 = 1;
    WPUBbits.WPUB1 = 1;
    
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1;
    
    // Configuracion interrupciones
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
}
