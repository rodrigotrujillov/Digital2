
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

#include "lcd.h"
#include "adc.h"
#include "uart.h"

int flag;
char opcion;
char dato;

float lectura_adc;
float voltaje;
char valor[20];

void cadena(char txt[]);
void escribir_terminal(char data);

void __interrupt() isr(void) {
    if (PIR1bits.RCIF) {
        dato = RCREG;
        __delay_us(5);
        PIR1bits.RCIF = 0;
    }
}

void main(void) {
    ANSEL = 0x01;
    ANSELH = 0x00;
    
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    // configuracion comunicacion serial
    TXSTAbits.SYNC = 0;             // comunicacion asincrona (full duplex)
    TXSTAbits.BRGH = 1;             // baudrate de alta velocidad
    BAUDCTLbits.BRG16 = 1;            // 16 bits para generar el baudrate

    SPBRG = 207;
    SPBRGH = 0;

    RCSTAbits.SPEN = 1;             // habilitamos comunicacion
    RCSTAbits.RX9 = 0;              // solo usaremos 8 bits
    TXSTAbits.TXEN = 1;             // habilitamos transmisor    
    RCSTAbits.CREN = 1;             // habilitamos receptor

    // configuracion oscilador
    OSCCONbits.IRCF = 0b111; 
    OSCCONbits.SCS = 1;      
    
    //
    adc_init(0);
    Lcd8_Init();
    //UART_Init();
    //UART_RX_config(9600);
    //UART_TX_condig(9600);
    
    
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;              // interrupciones de recepcion
    INTCONbits.PEIE = 1;            // interrupciones de puertos
    INTCONbits.GIE = 1;             // interrupciones globales
    // loop
    while(1)
    {
        
        if(ADCON0bits.GO == 0)
        {
            __delay_us(50);
            ADCON0bits.GO = 1;
        }
        
        //cadena("alo");
        
        voltaje = map(ADRESH, 0, 255, 0, 5);
        
        sprintf(valor, "%.2f", voltaje);
        
        Lcd8_Set_Cursor(1,2);
        Lcd8_Write_String("Pot:");
        Lcd8_Set_Cursor(2,1);
        Lcd8_Write_String(valor);
        Lcd8_Set_Cursor(2,6);
        Lcd8_Write_String("V");
        Lcd8_Set_Cursor(1,10);
        Lcd8_Write_String("CPU:");
        Lcd8_Set_Cursor(2,10);
        Lcd8_Write_String("data");
        
        /*if (PIR1bits.RCIF == 1) {
            dato = RCREG;
            __delay_ms(5);
        }*/
        
        if (dato == '+'){
            PORTB++;
            dato = 0;
        } else if (dato == '-'){
            PORTB--;
            dato = 0;
        }
        
    }
    return;
}

void cadena(char txt[]) {
    int i = 0;
    while (txt[i] != '\0'){ // recorremos la cadena de caracteres
        while (TXIF != 1);  // verifica si el registro de transmision esta vacio
        TXREG = txt[i]; // enviar el caracter i
        i++;            // aumenta i
    }
}

void escribir_terminal(char data) {
    // verificamos si se han transmitido datos
    while (!TXSTAbits.TRMT); 
    TXREG = data;
}