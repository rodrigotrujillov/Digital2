/* 
 * File:   LCD8bits.h
 * Author: rodri
 *
 * Created on 26 de julio de 2023, 11:26 PM
 */

#ifndef LCD8BITS_H
#define	LCD8BITS_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

#ifndef RS
#define RS RE0
#endif

#ifndef EN
#define EN RE1
#endif

#include <xc.h> // include processor files - each processor file is guarded.  

void Lcd_Port(char a);

void Lcd_Cmd(char a);

void Lcd_Clear(void);

void Lcd_Set_Cursor(char a, char b);

void Lcd_Init(void);

void Lcd_Write_Char(char a);

void Lcd_Write_String(char *a);

void Lcd_Shift_Right(void);

void Lcd_Shift_Left(void);

#endif	/* LCD8BITS_H */
