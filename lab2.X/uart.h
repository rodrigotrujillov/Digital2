#ifndef UART_H
#define	UART_H

#include <xc.h> // include processor files - each processor file is guarded.  

void UART_Init(); 

void UART_RX_config(uint16_t baudrate);

void UART_TX_condig(uint16_t baudrate);

void UART_write_char(char c);

char UART_read_char();

#endif	/* UART_H */

