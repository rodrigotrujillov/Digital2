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

int adc_read(){
    return ADRESH;
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

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

