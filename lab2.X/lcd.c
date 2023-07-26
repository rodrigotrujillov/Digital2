/* 
 * File: LCD.c  
 * Se utilizó y se adaptaron las librerías de Ligo George 
 * de la página www.electrosome.com
 * Enlace: https://electrosome.com/lcd-pic-mplab-xc8/
 * Revision history: 
 */

//LCD Functions Developed by electroSome

#include "LCD.h"

void Lcd_Port(char a) {
    (a & 1) ? (D4 = 1) : (D4 = 0);
    (a & 2) ? (D5 = 1) : (D5 = 0);
    (a & 4) ? (D6 = 1) : (D6 = 0);
    (a & 8) ? (D7 = 1) : (D7 = 0);
}

void Lcd_Cmd(char a) {
    RS = 0; // => RS = 0 // Dato en el puerto lo va interpretar como comando
    Lcd_Port(a);
    EN = 1; // => E = 1
    __delay_ms(4);
    EN = 0; // => E = 0
}

void Lcd_Clear(void) {
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b) {
    char temp, z, y;
    if (a == 1) {
        temp = 0x80 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
}

void Lcd_Init(void) {
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x03);
    __delay_ms(5);
    Lcd_Cmd(0x03);
    __delay_ms(11);
    Lcd_Cmd(0x03);
    /////////////////////////////////////////////////////
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x08);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char a) {
    char temp, y;
    temp = a & 0x0F;
    y = a & 0xF0;
    RS = 1; // => RS = 1
    Lcd_Port(y >> 4); //Data transfer
    EN = 1;
    __delay_us(40);
    EN = 0;
    Lcd_Port(temp);
    EN = 1;
    __delay_us(40);
    EN = 0;
}

void Lcd_Write_String(char *a) {
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right(void) {
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x0C);
}

void Lcd_Shift_Left(void) {
    Lcd_Cmd(0x01);
    Lcd_Cmd(0x08);
}


// 8 Bits **********************************************************************

void Lcd8_Port(char a){
    PORTD = a;
}

void Lcd8_Cmd(char a){
    RS8 = 0;
    Lcd8_Port(a);
    EN8 = 1;
    __delay_ms(40);
    EN8 = 0;
}

void Lcd8_Clear(void){
    Lcd8_Cmd(0);
    Lcd8_Cmd(1);
}

void Lcd8_Set_Cursor(char a, char b) {
    if (a == 1) {
        Lcd8_Cmd(0x80 + b - 1);
    } else if (a == 2) {
        Lcd8_Cmd(0xC0 + b - 1);
    }       
}

void Lcd8_Init(void){
    __delay_ms(15);
    Lcd8_Port(0x00);
    __delay_ms(20);
    Lcd8_Cmd(0x30);
    __delay_ms(5);
    Lcd8_Cmd(0x30);
    __delay_ms(160);
    Lcd8_Cmd(0x30);
    /////////////////////////////////////////////////////
    Lcd8_Cmd(0x38);
    Lcd8_Cmd(0x10);
    Lcd8_Cmd(0x01);
    Lcd8_Cmd(0x06);
    Lcd8_Cmd(0x0C);
}

void Lcd8_Write_Char(char a){
    RS8 = 1; 
    Lcd8_Port(a); 
    EN8 = 1;
    __delay_us(40);
    EN8 = 0;
}

void Lcd8_Write_String(char *a){
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd8_Write_Char(a[i]);
}
 