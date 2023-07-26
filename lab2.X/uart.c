#include "uart.h" 

void UART_Init() {
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCTLbits.BRG16 = 1;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
}

void UART_RX_config(uint16_t baudrate) {
    SPBRG = (8000000/(4*baudrate)) - 1;
    RCSTAbits.CREN = 1;
}

void UART_TX_condig(uint16_t baudrate) {
    SPBRGH = 0;
    TXSTAbits.TXEN = 1;
}

void UART_write_char(char c) {
    while (c != '\0'){
        while(TXSTAbits.TRMT == 0);
        TXREG = c;
        c++;
    }
}

char UART_read_char(){
    return RCREG;
}
