/*
 * File:   Proyecto1_Slave3.c
 * Author: rodri
 *
 * Created on 22 de agosto de 2023, 07:25 PM
 */
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


#include "I2C.h"

//CONSTANTES
#define _XTAL_FREQ 8000000

//VARIABLES

#define trigger PORTCbits.RC0 
#define echo PORTCbits.RC1

char buff[16];
int dist;
uint8_t z;
uint8_t dato;

int ObtenerDistancia(void)
{
    int distancia = 0;
    
    trigger = 1;
    __delay_us(10);
    trigger = 0;
    
    
    while(echo == 0);
    TMR1ON = 1;
    while(echo == 1);
    TMR1ON = 0;
    
    distancia = (TMR1L | (TMR1H<<8));
    distancia = distancia/58.82;
    distancia = distancia + 1;
    return distancia;
}


//PROTOTIPO DE FUNCIONES
void setup(void);

//INTERRUPCIONES
void __interrupt() isr(void){
   if(PIR1bits.SSPIF == 1){ 

        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            PORTB = SSPBUF;             // Guardar en el PORTB el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            I2C_Write_Slave(dist);
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
}
void servoRotate180(){
    unsigned int i;
    for(i=0;i<50;i++)
    {
        PORTBbits.RB7 = 1;
        __delay_us(800);
        PORTBbits.RB7 = 0;
        __delay_us(19200);
    }
}

void servoRotate0(){
    unsigned int i;
    for(i=0;i<50;i++)
    {
        PORTBbits.RB7 = 1;
        __delay_us(2200);
        PORTBbits.RB7 = 0;
        __delay_us(17800);
    }
}

//MAIN
void main(void) {
    setup();
    I2C_Init_Slave(0x70);
    while(1){
        TMR1H = 0;
        TMR1L = 0;
        
        RC0 = 1;
        __delay_us(10);
        RC0 = 0;
        
        dist = ObtenerDistancia();
        if (dist < 20){
            PORTDbits.RD0 = 1;
            servoRotate180();
            __delay_ms(5000);
        }
        else{
            PORTDbits.RD0 = 0;
            servoRotate0();
            
        }
    __delay_ms(400);
    }
    return;
}

//CONFIGURACIONES
void setup(void){
    TRISC0 = 0;
    RC0 = 0;
    TRISC1= 1;
 
    T1CON = 0x10;
   
    ANSEL = 0x00;
    ANSELH = 0x00;
    
    TRISB = 0x00;
    PORTB = 0x00;
    
    TRISD = 0x00;
    PORTD = 0x00;
    
    T1CKPS0 = 0;
    T1CKPS1 = 0;
    T1CKPS0 = 0;

   
    // Confiiguración del oscilador
    OSCCONbits.IRCF = 0b111; //Config. de oscilacion 8MHz
    OSCCONbits.SCS = 1;      //reloj interno
    
    // COnfiguración de interrupciones
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupción MSSP
   
    //Configuración I2C
   
}
