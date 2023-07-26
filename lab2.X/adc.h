#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  

void adc_init(int channel);

int adc_read();

void adc_change_channel(int channel);

int adc_get_channel();

float map(float x, float in_min, float in_max, float out_min, float out_max);


#endif	/* ADC_H */

