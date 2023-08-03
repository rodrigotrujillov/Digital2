
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

#include "adc.h"
#include "SPI.h"

//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
uint8_t pot_slave1;

//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
    if(SSPIF == 1){
        z = spiRead();      // leemos valor anterior del buffer 
        spiWrite(pot_slave1);   // cargamos el valor de pot_slave1 al buffer 
        SSPIF = 0;
    }
}

//*****************************************************************************
// Prototipos de funciones
//*****************************************************************************
void setup(void);

//*****************************************************************************
// Código Principal
//*****************************************************************************
void main(void){
    setup();
    adc_init(0);
    
    while(1){
        if(ADCON0bits.GO == 0)
        {
            pot_slave1 = ADRESH;
            __delay_us(50);
            ADCON0bits.GO = 1;
        }
    }
    return;
}

//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0x01;
    ANSELH = 0;
  
    TRISB = 0;
    PORTB = 0;
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIR1bits.SSPIF = 0;
    PIE1bits.SSPIE = 1;
    TRISAbits.TRISA5 = 1;
 
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
}