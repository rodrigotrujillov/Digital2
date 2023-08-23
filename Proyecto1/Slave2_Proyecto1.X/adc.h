/* 
 * File:   adc.h
 * Author: rodri
 *
 * Created on 22 de agosto de 2023, 01:58 PM
 */

#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

void adc_init(char Select);

void adc_change_channel(uint8_t canal);

int adc_get_channel(void);

int adc_read(void);

#endif	/* ADC_H */

