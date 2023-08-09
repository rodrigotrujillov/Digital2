#include "adc.h"
#define _XTAL_FREQ 8000000

int adc_result;
int channel;

void adc_init(int channel){
    ADCON1bits.ADFM = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 1;
    ADCON0bits.CHS = channel;
    __delay_us(100);
    ADCON0bits.ADON = 1;

    ADCON0bits.GO = 1;
}

unsigned int adc_read(unsigned int channel){
    ADCON0bits.CHS = channel; // Select the input channel
    ADCON0bits.GO = 1;       // Start the conversion
    while(ADCON0bits.GO);    // Wait for the conversion to complete
    __delay_us(100);
    return (ADRESH); // Return the result
   
}

void adc_change_channel(int channel){
    if (ADCON0bits.GO == 0){
        if (ADCON0bits.CHS == channel) 
            ADCON0bits.CHS = adc_get_channel();
        else channel;
        __delay_us(1000);
        ADCON0bits.GO = 1;  
    }  
}

int adc_get_channel(){
    return ADCON0bits.CHS;
}

