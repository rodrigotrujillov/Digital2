#include "tabla.h"

uint8_t tabla(uint8_t valor){
    switch(valor)
    {
        case 0:
            return 0b00111111;  // 0
            break;
        case 1:
            return 0b00000110;  // 1
            break;
        case 2:
            return 0b01011011;  // 2
            break;
        case 3:
            return 0b01001111;  // 3
            break;
        case 4:
            return 0b01100110;  // 4
            break;
        case 5:
            return 0b01101101;  // 5
            break;
        case 6:
            return 0b01111101;  // 6
            break;
        case 7:
            return 0b00000111;  // 7
            break;
        case 8:
            return 0b01111111;  // 8
            break;
        case 9:
            return 0b01101111;  // 9
            break;
        case 10:                // A
            return 0b01110111;
            break;
        case 11:                // B
            return 0b01111100;
            break;
        case 12:                // C
            return 0b00111001;
            break;
        case 13:                // D
            return 0b01011110;
            break;
        case 14:                // E
            return 0b01111001;
            break;
        case 15:                // F
            return 0b01110001;
            break;
        default:                // PUNTO
            return 0b10000000;  
            break;
    }
}