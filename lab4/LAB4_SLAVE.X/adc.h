#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  

void adc_init(int channel);

unsigned int adc_read(unsigned int channel);

void adc_change_channel(int channel);

int adc_get_channel();

#endif	/* ADC_H */

