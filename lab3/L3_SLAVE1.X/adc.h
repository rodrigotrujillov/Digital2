#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  

void adc_init(int channel);

int adc_read();

void adc_change_channel(int channel);

int adc_get_channel();

#endif	/* ADC_H */

