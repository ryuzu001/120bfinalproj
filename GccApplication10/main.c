/*	
 *	Ryan Yuzuki ryuzu001@ucr.edu
 *	Lab Section: 024
 *	
 *  Final project
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>	
#include "io.c"
#include "timer.h"

char hello[50] = "Hello World abcdefghijklmnop";
char world[50] = "World!";

void LCD_Custom_Char (unsigned char loc, unsigned char *msg)
{
	int i;
	LCD_WriteCommand (0x40 + (loc*8));	/* Command 0x40 and onwards forces the device to point CGRAM address */
	for(i=0;i<7;i++){	/* Write 8 byte for generation of 1 character */
		LCD_WriteData(msg[i]);
	}
	LCD_WriteCommand(0x80);
}

int main() {
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	//unsigned char Character1[8] = { 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00 }; 
	// unsigned char Character1;
	unsigned char battery[7] = {0x0e, 0x0a, 0x1b, 0x11, 0x11, 0x11, 0x1f};
	/* Custom char set for alphanumeric LCD Module */
	
	
	
	LCD_init();
	
	LCD_WriteCommand(0x00);
	LCD_Custom_Char(0, battery);  /* Build Character1 at position 0 */
	LCD_WriteData(0);
	
	//LCD_ClearScreen();
	// LCD_DisplayString(1, hello);
	while(1);
}


