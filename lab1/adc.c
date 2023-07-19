#include "adc.h"
#define _XTAL_FREQ 8000000

int adc_result;
int channel;

void adc_init(int channel){
    ADCON1bits.ADFM = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS = 0b10;
    ADCON0bits.CHS = channel;
    ADCON0bits.ADON = 1;
    __delay_us(50);
    ADIF = 0;
    adc_get_channel();
}

int adc_read(){
    adc_result = ADRESH;
    return adc_result;
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
     ADCON0bits.CHS = channel;
    return channel;
}
