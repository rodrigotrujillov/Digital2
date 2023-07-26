
#pragma config FOSC = EXTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
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

#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#define _XTAL_FREQ 8000000
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RB5
#define D6 RD6
#define D7 RD7

#include "LCD.h"
#include "adc.h"

char valor[20];

void main(void) {
    ANSEL = 0x01;
    ANSELH = 0x00;
    
    TRISB = 0x00;
    
    TRISD = 0x00;
    
    OSCCONbits.IRCF = 0b111; 
    OSCCONbits.SCS = 1;      
    
    adc_init(0);
    Lcd_Init();
    while(1)
    {
        
        if(ADCON0bits.GO == 0)
        {
            PORTB = ADRESH;
            __delay_us(50);
            ADCON0bits.GO = 1;
        }
        
        
        sprintf(valor, "%d", PORTB);
        
        Lcd_Set_Cursor(1,1);
        Lcd_Write_String("Pot1:");
        Lcd_Set_Cursor(2,1);
        Lcd_Write_String(valor);
    }
    return;
}
