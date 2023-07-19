#include "timer0.h"

void timer0_set(char prescaler, char tmr0_val) {
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    switch(prescaler) {
        case 2:
            OPTION_REGbits.PS = 000;
            break;
        case 4:
            OPTION_REGbits.PS = 001;
            break;
        case 8:
            OPTION_REGbits.PS = 010;
            break;
        case 16:
            OPTION_REGbits.PS = 011;
            break;
        case 32:
            OPTION_REGbits.PS = 100;
            break;
        case 64:
            OPTION_REGbits.PS = 101;
            break;
        case 128:
            OPTION_REGbits.PS = 110;
            break;
        case 256:
            OPTION_REGbits.PS = 111;
            break;
    } 
    
    TMR0 = tmr0_val;
}

/*
 EJEMPLO:
 * 
 * SE NECESITA UN PRESCALER DE 1:64, CON UN VALOR DE TMR0 DE 100
 * SE ESCRIBIRIA DE LA SIGUIENTE MANERA
 * 
 * timer0_set(64, 100);
 * 
 */