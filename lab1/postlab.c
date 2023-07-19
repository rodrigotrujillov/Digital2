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
#include "iocb.h"
#include "adc.h"
#include "timer0.h"
#include "tabla.h"

//******************************************************************************
// Constantes y variables
//******************************************************************************

#define _XTAL_FREQ 4000000
#define aumentar PORTBbits.RB0
#define disminuir PORTBbits.RB1
#define alarma PORTBbits.RB7
#define LEDs PORTC
#define Display PORTD
#define tmr0_value 6

uint8_t flag = 0;
uint8_t valor_adresh;
uint8_t decena, unidad;
uint8_t display_unidad, display_decena;

//******************************************************************************
// Prototipos
//******************************************************************************

void setup(void);

//******************************************************************************
// Interrupciones
//******************************************************************************

void __interrupt() isr(void) {
    
    
    if (PIR1bits.ADIF) {
        valor_adresh = adc_read();
        PIR1bits.ADIF = 0;
    }
    
    // interrupcion RB para contador
    if (INTCONbits.RBIF) {
        if (!aumentar) {
            LEDs++;
        }
        if (!disminuir) {
            LEDs--;
        }
        INTCONbits.RBIF = 0;
    }
    
    if(INTCONbits.T0IF){
        PORTE = 0;
        switch (flag) {
            case 0:
                PORTE = 0b01;
                Display = display_decena;
                flag = 1;
                break;
            case 1:
                PORTE = 0b10;
                Display = display_unidad;
                flag = 0;
                break;
        }
        
        
        TMR0 = tmr0_value;
        INTCONbits.T0IF = 0;
    }
}

//******************************************************************************
// MAIN
//******************************************************************************

void main(void) {
    adc_init(0);
    ioc_init(0);
    ioc_init(1);
    timer0_set(4, tmr0_value);
    setup();
    ADCON0bits.GO = 1;
    while(1){
        if (ADCON0bits.GO == 0) {
            __delay_us(50);
            ADCON0bits.GO = 1;
        }
        
        decena = valor_adresh/16;
        unidad = valor_adresh%16;
        
        display_decena = tabla(decena);
        display_unidad = tabla(unidad);
        
        if (valor_adresh > LEDs) alarma = 1;
        else alarma = 0;
    }
    return;
}

//******************************************************************************
// Setup
//******************************************************************************

void setup(void){
    // Configuracion de puertos
    ANSEL = 0b00000001;
    ANSELH = 0;
    
    TRISB = 0b00000011;
    PORTB = 0;
    
    TRISC = 0;
    PORTC = 0;
    
    TRISD = 0;
    PORTD = 0;
    
    TRISE = 0;
    PORTE = 0;
    
    // Configuracion oscilador
    OSCCONbits.IRCF = 0b110;
    OSCCONbits.SCS = 1;
    
    // Configuracion interrupciones
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    INTCONbits.RBIE = 1;
    INTCONbits.RBIF = 0;
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    
}
