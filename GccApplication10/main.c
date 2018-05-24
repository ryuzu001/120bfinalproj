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

void displayMenu();

void LCD_Custom_Char (unsigned char loc, unsigned char *msg)
{
	int i;
	LCD_WriteCommand (0x40 + (loc*8));	/* Command 0x40 for CGRAM */
	for(i=0;i<7;i++){	/* 7 cause the bottom is reserved for the cursor */
		LCD_WriteData(msg[i]);
	}
	LCD_WriteCommand(0x80);
}

void displayMenu(){
	// select difficulty
	// set time
	// play game
	// something else here
	
}

int main() {
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	//unsigned char block1[7] = { 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f}; 
		
	unsigned char crosshatch[7] = { 0x15, 0x0a, 0x15, 0x0a, 0x15, 0x0a, 0x15}; 	
	unsigned char cross[7] = {0x00, 0x0e, 0x1b, 0x11, 0x1b, 0x0e, 0x00 };
	unsigned char smiley[7] = {0x00, 0x11, 0x00, 0x04, 0x11, 0x0e, 0x00};	
		
	unsigned char selH[7] = {0x1f, 0x15, 0x15, 0x11, 0x15, 0x15, 0x1f};	
	unsigned char selE[7] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f};
	unsigned char selL[7] = {0x1f, 0x17, 0x17, 0x17, 0x17, 0x11, 0x1f};
	unsigned char selO[7] = {0x1f, 0x11, 0x15, 0x15, 0x15, 0x11, 0x1f};
		
	/* Custom char set for alphanumeric LCD Module */
	
	
	
	LCD_init();
	
	/*
	LCD_WriteCommand(0x01);
	LCD_Custom_Char(1, battery);
	LCD_WriteData(1);*/
	
	
	// LCD_DisplayString(1, hello);
	
	LCD_WriteCommand(0x38); /* function set */
	LCD_WriteCommand(0x0c); /* display on,cursor off,blink off */
	TimerSet(1000);
	TimerOn(); 
	
	LCD_ClearScreen();
	while(1){	
		unsigned char cur= 1;
		LCD_Custom_Char(0, crosshatch);  // Build cross at position 0 
		LCD_Custom_Char(1, cross);
		LCD_Custom_Char(2, smiley);
		LCD_Custom_Char(3, selH);
		LCD_Custom_Char(4, selE);
		LCD_Custom_Char(5, selL);
		LCD_Custom_Char(6, selO);
		
		LCD_Cursor(cur++);
		LCD_WriteData(0);			  // Writes position 0 
		LCD_Cursor(cur++);
		LCD_WriteData(' ');
		LCD_Cursor(cur++);
		LCD_WriteData(1);
		LCD_Cursor(cur++);
		LCD_WriteData(' ');
		LCD_Cursor(cur++);
		LCD_WriteData(2);
		LCD_Cursor(cur++);
		LCD_WriteData(' ');
		LCD_Cursor(cur++);
		LCD_WriteData('H');
		LCD_Cursor(cur++);
		LCD_WriteData('E');
		LCD_Cursor(cur++);
		LCD_WriteData('L');
		LCD_Cursor(cur++);
		LCD_WriteData('L');
		LCD_Cursor(cur++);
		LCD_WriteData('O');
		LCD_Cursor(cur++);
		LCD_WriteData(' ');
		LCD_Cursor(cur++);
		LCD_WriteData(3);
		LCD_Cursor(cur++);
		LCD_WriteData(4);
		LCD_Cursor(cur++);
		LCD_WriteData(5);
		LCD_Cursor(cur++);
		LCD_WriteData(5);
		LCD_Cursor(cur++);
		LCD_WriteData(6);
		LCD_Cursor(cur++);
		LCD_WriteData(' ');
		LCD_Cursor(cur++);
		LCD_WriteData(3);
		LCD_Cursor(cur++);
		LCD_WriteData(4);
		LCD_Cursor(cur++);
		LCD_WriteData(5);
		LCD_Cursor(cur++);
		LCD_WriteData(5);
		LCD_Cursor(cur++);
		LCD_WriteData(6);	
		
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
}


