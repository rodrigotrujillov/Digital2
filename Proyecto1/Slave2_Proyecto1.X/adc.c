#include <xc.h>
#include <stdint.h>
#include "adc.h"
#define _XTAL_FREQ 8000000



void adc_init(char Select){
    
    ADCON0bits.ADCS = 0b10;     // ADCS <1:0> -> 10 FOSC/32
    ADCON0bits.ADON = Select;        // Encender ADC
    ADCON1bits.ADFM = 0;
    ADCON1bits.VCFG0 = 0;       // VDD
    ADCON1bits.VCFG1 = 0;       // VSS
    __delay_us(50);
    ADCON0bits.GO = 1;
    
   
}

void adc_change_channel(uint8_t canal){
    switch(canal){
        case 0:
            ADCON0bits.CHS = 0b0000;
            break;
        case 1:
            ADCON0bits.CHS = 0b0001;
            break;
        case 2:
            ADCON0bits.CHS = 0b0010;
            break;
        case 3:
            ADCON0bits.CHS = 0b0011;
            
    }
   
    
}

int adc_read(void){
    PIR1bits.ADIF = 0;          
    return (ADRESH);            
}
