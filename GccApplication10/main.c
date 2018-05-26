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
#include "bit.h"
#include "timer.h"

enum mainMenu{init, topLeft, topRight, bottomLeft, bottomRight} mm;

void displayMenu();
void dispTopLeft();
void dispTopRight();
void dispBottomLeft();
void dispBottomRight();
void topLeftSelect();
void topRightSelect();
void bottomLeftSelect();
void bottomRightSelect();

void LCD_Custom_Char (unsigned char loc, unsigned char *msg){
	int i;
	LCD_WriteCommand (0x40 + (loc*8));	/* Command 0x40 for CGRAM */
	for(i = 0;i < 8; i++)	/* 7 cause the bottom is reserved for the cursor */
		LCD_WriteData(msg[i]);
	LCD_WriteCommand(0x80);
}
void dispTopRight(){
	// PLAY highlighted in the top left
	
	unsigned char selD[8] = {0x1f, 0x13, 0x15, 0x15, 0x15, 0x13, 0x1f, 0x00};
	unsigned char selI[8] = {0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x11, 0x1f, 0x00};
	unsigned char selF[8] = {0x1f, 0x11, 0x17, 0x11, 0x17, 0x17, 0x1f, 0x00};
	unsigned char selC[8] = {0x1f, 0x11, 0x17, 0x17, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selU[8] = {0x1f, 0x15, 0x15, 0x15, 0x15, 0x11, 0x1f, 0x00};
	unsigned char selL[8] = {0x1f, 0x17, 0x17, 0x17, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selT[8] = {0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x1b, 0x1f, 0x00};
	unsigned char selY[8] = {0x1f, 0x15, 0x15, 0x1b, 0x1b, 0x1b, 0x1f, 0x00};
	
	LCD_Custom_Char(0, selD);		// build P at position 0...
	LCD_Custom_Char(1, selI);
	LCD_Custom_Char(2, selF);
	LCD_Custom_Char(3, selC);
	LCD_Custom_Char(4, selU);
	LCD_Custom_Char(5, selL);
	LCD_Custom_Char(6, selT);
	LCD_Custom_Char(7, selY);
	
	unsigned char cur = 1;		// initialize cursor to 1
	
	LCD_Cursor(cur++);
	LCD_WriteData('P');
	LCD_Cursor(cur++);
	LCD_WriteData('L');
	LCD_Cursor(cur++);
	LCD_WriteData('A');
	LCD_Cursor(cur++);
	LCD_WriteData('Y');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(0);
	LCD_Cursor(cur++);
	LCD_WriteData(1);
	LCD_Cursor(cur++);
	LCD_WriteData(2);
	LCD_Cursor(cur++);
	LCD_WriteData(2);
	LCD_Cursor(cur++);
	LCD_WriteData(1);
	LCD_Cursor(cur++);
	LCD_WriteData(3);
	LCD_Cursor(cur++);
	LCD_WriteData(4);
	LCD_Cursor(cur++);
	LCD_WriteData(5);
	LCD_Cursor(cur++);
	LCD_WriteData(6);
	LCD_Cursor(cur++);
	LCD_WriteData(7);
	LCD_Cursor(cur++);		// Newline here
	LCD_WriteData('T');
	LCD_Cursor(cur++);
	LCD_WriteData('I');
	LCD_Cursor(cur++);
	LCD_WriteData('M');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('2');
	LCD_Cursor(cur++);
	LCD_WriteData('P');
	LCD_Cursor(cur++);
	LCD_WriteData('L');
	LCD_Cursor(cur++);
	LCD_WriteData('A');
	LCD_Cursor(cur++);
	LCD_WriteData('Y');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('R');
}
void dispTopLeft(){
	// PLAY highlighted in the top left
	
	unsigned char selP[8] = {0x1f, 0x11, 0x15, 0x11, 0x17, 0x17, 0x1f, 0x00};
	unsigned char selL[8] = {0x1f, 0x17, 0x17, 0x17, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selA[8] = {0x1f, 0x1b, 0x15, 0x11, 0x15, 0x15, 0x1f, 0x00};
	unsigned char selY[8] = {0x1f, 0x15, 0x15, 0x1b, 0x1b, 0x1b, 0x1f, 0x00};
		
	LCD_Custom_Char(0, selP);		// build P at position 0...
	LCD_Custom_Char(1, selL);
	LCD_Custom_Char(2, selA);
	LCD_Custom_Char(3, selY);
	
	unsigned char cur = 1;		// initialize cursor to 1
	
	LCD_Cursor(cur++);
	LCD_WriteData(0);
	LCD_Cursor(cur++);
	LCD_WriteData(1);
	LCD_Cursor(cur++);
	LCD_WriteData(2);
	LCD_Cursor(cur++);
	LCD_WriteData(3);
	LCD_Cursor(cur++);
	LCD_WriteData(' ');	
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('D');
	LCD_Cursor(cur++);
	LCD_WriteData('I');
	LCD_Cursor(cur++);
	LCD_WriteData('F');
	LCD_Cursor(cur++);
	LCD_WriteData('F');
	LCD_Cursor(cur++);
	LCD_WriteData('I');
	LCD_Cursor(cur++);
	LCD_WriteData('C');
	LCD_Cursor(cur++);
	LCD_WriteData('U');
	LCD_Cursor(cur++);
	LCD_WriteData('L');
	LCD_Cursor(cur++);
	LCD_WriteData('T');
	LCD_Cursor(cur++);
	LCD_WriteData('Y');
	LCD_Cursor(cur++);		// Newline here
	LCD_WriteData('T');
	LCD_Cursor(cur++);
	LCD_WriteData('I');
	LCD_Cursor(cur++);
	LCD_WriteData('M');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('2');
	LCD_Cursor(cur++);
	LCD_WriteData('P');
	LCD_Cursor(cur++);
	LCD_WriteData('L');
	LCD_Cursor(cur++);
	LCD_WriteData('A');
	LCD_Cursor(cur++);
	LCD_WriteData('Y');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('R');
}
void displayMenu(){
	// select difficulty
	// set time
	// play game
	// 2player
	
	// PINA: 0 up
	// PINA: 1 right
	// PINA: 2 down
	// PINA: 3 left
	
	
	switch(mm){			// transitions
		case init:
			mm = topLeft;
		break;
		case topLeft:
			if(!GetBit(PINA, 1)){
				mm = topRight;
			}
			else if(!GetBit(PINA, 2)){
				mm = bottomLeft;
			}
			else{
				mm = topLeft;
			}
		break;
		case topRight:
			if(!GetBit(PINA, 3)){
				mm = topLeft;
			}
			else if(!GetBit(PINA, 2)){
				mm = bottomRight;
			}
			else{
				mm = topRight;
			}
		break;
		case bottomLeft:
			if(!GetBit(PINA, 0)){
				mm = topLeft;
			}
			else if(!GetBit(PINA, 1)){
				mm = bottomRight;
			}
			else{
				mm = bottomLeft;
			}
		break;
		case bottomRight:
			if(!GetBit(PINA, 0)){
				mm = topRight;
			}
			else if(!GetBit(PINA, 3)){
				mm = bottomLeft;
			}
			else{
				mm = bottomRight;
			}
		break;
		default:
		break;
	}
	switch(mm){			// actions
		case init:
		break;
		case topLeft:
			
		break;
		case topRight:
		break;
		case bottomLeft:
		break;
		case bottomRight:
		break;
		default:
		break;
	}
}

int main() {
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	//unsigned char block1[7] = { 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f}; 
	/*
	unsigned char crosshatch[7] = { 0x15, 0x0a, 0x15, 0x0a, 0x15, 0x0a, 0x15}; 	
	unsigned char cross[7] = {0x00, 0x0e, 0x1b, 0x11, 0x1b, 0x0e, 0x00 };
	unsigned char smiley[7] = {0x00, 0x11, 0x00, 0x04, 0x11, 0x0e, 0x00};	
		
	unsigned char selH[7] = {0x1f, 0x15, 0x15, 0x11, 0x15, 0x15, 0x1f};	
	unsigned char selE[7] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f};
	unsigned char selL[7] = {0x1f, 0x17, 0x17, 0x17, 0x17, 0x11, 0x1f};
	unsigned char selO[7] = {0x1f, 0x11, 0x15, 0x15, 0x15, 0x11, 0x1f};
		*/
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
		/*
		mm = init;
		
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
		*/
		// dispTopLeft();
		dispTopRight();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}


