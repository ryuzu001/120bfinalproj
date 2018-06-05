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
#include <avr/eeprom.h>	
#include "io.c"
#include "bit.h"
#include "timer.h"
#include "nokia5110.c"

#define CLOCK		PORTA7
#define LATCH		PORTA6
#define DATA		PORTA5
#define SNES_PORT	PORTA
#define SNES_PIN	PINA

enum mainMenu{init, topLeft, topRight, bottomLeft, bottomRight} mm;
enum difficultyMenu{initDif, easy, medium, hard} diff;
enum timeMenu{initTime, sec30, sec60, sec90} time_s;
	
unsigned char EEMEM eeprom_array[10];		// array to hold eeprom values.
// values: eeprom[0] is the difficulty
//		   eeprom[1] is the time
	
unsigned char difficulty = 1;	// diffculty - 0 easy, 1 medium 2 hard. default to medium.

unsigned char gameTime = 60;	// gametime in seconds

unsigned char inMenu = 1;		// start in the main menu
unsigned char inDifficulty = 0;
unsigned char inTime = 0;
unsigned char inGame = 0;

void displayMenu();
void dispTopLeft();
void dispTopRight();
void dispBottomLeft();
void dispBottomRight();
void LCD_Custom_Char (unsigned char loc, unsigned char *msg);

void setDifficulty();
void dispDifEasy();
void dispDifMedium();
void dispDifHard();
void setTime();
void dispTime30();
void dispTime60();
void dispTime90();

void SNES_init();
unsigned char readSNES();
void eraseEEPROM();

//////////////////////////////

void setTime(){
	// PINA: 0 up
	// PINA: 1 right
	// PINA: 2 down
	// PINA: 3 left
	// PINA: 4 select
	switch(time_s){
		case initTime:
		if(gameTime == 30)
			time_s = sec30;
		else if(gameTime == 60)
			time_s = sec60;
		else
			time_s = sec90;
		break;
		case sec30:
			if(!GetBit(PINA, 1)){
				time_s = sec60;
			}
			else if(!GetBit(PINA, 2)){
				time_s = sec90;
			}
			else if(!GetBit(PINA, 4)){
				while(!GetBit(PINA, 4));
				gameTime = 30;
				eeprom_write_word(&eeprom_array[0], difficulty);
				eeprom_write_word(&eeprom_array[1], gameTime);
				
				inTime = 0;
				inMenu = 1;
			}
			else
				time_s = sec30;
		break;
		case sec60:
			if(!GetBit(PINA, 3)){
				time_s = sec30;
			}
			else if(!GetBit(PINA, 2)){
				time_s = sec90;
			}
			else if(!GetBit(PINA, 4)){
				while(!GetBit(PINA, 4));
				gameTime = 60;
				eeprom_write_word(&eeprom_array[0], difficulty);
				eeprom_write_word(&eeprom_array[1], gameTime);
				
				inTime = 0;
				inMenu = 1;
			}
			else
			time_s = sec60;
		break;
		case sec90:
			if(!GetBit(PINA, 0)){
				time_s = sec30;
			}
			else if(!GetBit(PINA, 1)){
				time_s = sec60;
			}
			else if(!GetBit(PINA, 4)){
				while(!GetBit(PINA, 4));
				gameTime = 90;
				eeprom_write_word(&eeprom_array[0], difficulty);
				eeprom_write_word(&eeprom_array[1], gameTime);
				
				inTime = 0;
				inMenu = 1;
			}
			else
			time_s = sec90;
		break;						
		default:
		break;
	}
	switch(time_s){
		case initTime:
			if(gameTime == 30)
				dispTime30();
			else if(gameTime == 60)
				dispTime60();
			else
				dispTime90();
		break;
		case sec30:
			dispTime30();
		break;
		case sec60:
			dispTime60();
		break;
		case sec90:
			dispTime90();
		break;
		default:
		break;			
	}
}
void dispTime30(){
	// 30sec highlighted in the top left
	
	unsigned char sel3[8] = {0x1f, 0x11, 0x1d, 0x11, 0x1d, 0x11, 0x1f, 0x00};
	unsigned char sel0[8] = {0x1f, 0x11, 0x15, 0x15, 0x15, 0x11, 0x1f, 0x00};
	unsigned char selS[8] = {0x1f, 0x11, 0x17, 0x11, 0x1d, 0x11, 0x1f, 0x00};
	unsigned char selE[8] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selC[8] = {0x1f, 0x11, 0x17, 0x17, 0x17, 0x11, 0x1f, 0x00};	
	
	LCD_Custom_Char(0, sel3);		// build at position 0...
	LCD_Custom_Char(1, sel0);
	LCD_Custom_Char(2, selS);
	LCD_Custom_Char(3, selE);
	LCD_Custom_Char(4, selC);
	
	unsigned char cur = 1;		// initialize cursor to 1
	
	LCD_Cursor(cur++);
	LCD_WriteData(0);
	LCD_Cursor(cur++);
	LCD_WriteData(1);
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(2);
	LCD_Cursor(cur++);
	LCD_WriteData(3);
	LCD_Cursor(cur++);
	LCD_WriteData(4);
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('6');
	LCD_Cursor(cur++);
	LCD_WriteData('0');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('S');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('C');
	LCD_Cursor(cur++);		// Newline here
	LCD_WriteData('9');
	LCD_Cursor(cur++);
	LCD_WriteData('0');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('S');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('C');
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
}
void dispTime60(){
	// 30sec highlighted in the top left
	
	unsigned char sel6[8] = {0x1f, 0x11, 0x17, 0x11, 0x15, 0x11, 0x1f, 0x00};
	unsigned char sel0[8] = {0x1f, 0x11, 0x15, 0x15, 0x15, 0x11, 0x1f, 0x00};
	unsigned char selS[8] = {0x1f, 0x11, 0x17, 0x11, 0x1d, 0x11, 0x1f, 0x00};
	unsigned char selE[8] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selC[8] = {0x1f, 0x11, 0x17, 0x17, 0x17, 0x11, 0x1f, 0x00};
	
	LCD_Custom_Char(0, sel6);		// build at position 0...
	LCD_Custom_Char(1, sel0);
	LCD_Custom_Char(2, selS);
	LCD_Custom_Char(3, selE);
	LCD_Custom_Char(4, selC);
	
	unsigned char cur = 1;		// initialize cursor to 1
	
	LCD_Cursor(cur++);
	LCD_WriteData('3');
	LCD_Cursor(cur++);
	LCD_WriteData('0');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('S');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('C');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(0);
	LCD_Cursor(cur++);
	LCD_WriteData(1);
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(2);
	LCD_Cursor(cur++);
	LCD_WriteData(3);
	LCD_Cursor(cur++);
	LCD_WriteData(4);
	LCD_Cursor(cur++);		// Newline here
	LCD_WriteData('9');
	LCD_Cursor(cur++);
	LCD_WriteData('0');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('S');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('C');
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
}
void dispTime90(){
	// 30sec highlighted in the top left
	
	unsigned char sel9[8] = {0x1f, 0x11, 0x15, 0x11, 0x1d, 0x11, 0x1f, 0x00};
	unsigned char sel0[8] = {0x1f, 0x11, 0x15, 0x15, 0x15, 0x11, 0x1f, 0x00};
	unsigned char selS[8] = {0x1f, 0x11, 0x17, 0x11, 0x1d, 0x11, 0x1f, 0x00};
	unsigned char selE[8] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selC[8] = {0x1f, 0x11, 0x17, 0x17, 0x17, 0x11, 0x1f, 0x00};
	
	LCD_Custom_Char(0, sel9);		// build at position 0...
	LCD_Custom_Char(1, sel0);
	LCD_Custom_Char(2, selS);
	LCD_Custom_Char(3, selE);
	LCD_Custom_Char(4, selC);
	
	unsigned char cur = 1;		// initialize cursor to 1
	
	LCD_Cursor(cur++);
	LCD_WriteData('3');
	LCD_Cursor(cur++);
	LCD_WriteData('0');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('S');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('C');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('6');
	LCD_Cursor(cur++);
	LCD_WriteData('0');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('S');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('C');
	LCD_Cursor(cur++);		// Newline here
	LCD_WriteData(0);
	LCD_Cursor(cur++);
	LCD_WriteData(1);
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(2);
	LCD_Cursor(cur++);
	LCD_WriteData(3);
	LCD_Cursor(cur++);
	LCD_WriteData(4);
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
}
void setDifficulty(){
	// PINA: 0 up
	// PINA: 1 right
	// PINA: 2 down
	// PINA: 3 left
	// PINA: 4 select

	switch(diff){	// transitions
		case initDif:
			if(difficulty == 0)
			diff = easy;
			else if(difficulty == 1)
			diff = medium;
			else 
			diff = hard;
		break;
		case easy:
			if(!GetBit(PINA, 1)){
				diff = medium;
			}
			else if(!GetBit(PINA, 2)){
				diff = hard;
			}
			else if(!GetBit(PINA, 4)){		// select
				while(!GetBit(PINA, 4));	// wait for release
				difficulty = 0;				// set to easy
				eeprom_write_word(&eeprom_array[0], difficulty);
				eeprom_write_word(&eeprom_array[1], gameTime);
				
				inDifficulty = 0;
				inMenu = 1;
			}
			else{
				diff = easy;
			}
		break;
		case medium:
			if(!GetBit(PINA, 3)){
				diff = easy;
			}
			else if(!GetBit(PINA, 2)){
				diff = hard;
			}
			else if(!GetBit(PINA, 4)){
				while(!GetBit(PINA, 4));	// wait for release
				difficulty = 1;		// medium
				eeprom_write_word(&eeprom_array[0], difficulty);
				eeprom_write_word(&eeprom_array[1], gameTime);
				
				inDifficulty = 0;
				inMenu = 1;
			}
			else{
				diff = medium;
			}
		break;
		case hard:
			if(!GetBit(PINA, 0)){
				diff = easy;
			}
			else if(!GetBit(PINA, 1)){
				diff = medium;
			}
			else if(!GetBit(PINA, 4)){
				while(!GetBit(PINA, 4));	// wait for release
				difficulty = 2;				// hard
				eeprom_write_word(&eeprom_array[0], difficulty);
				eeprom_write_word(&eeprom_array[1], gameTime);
				
				inDifficulty = 0;
				inMenu = 1;
			}
			else{
				diff = hard;
			}
		break;
		default:
		break;
	}
	switch(diff){	// actions
		case initDif:
			if(difficulty == 0){
				dispDifEasy();
			}
			else if(difficulty == 1){
				dispDifMedium();
			}
			else{
				dispDifHard();
			}
		break;
		case easy:
			dispDifEasy();
		break;
		case medium:
			dispDifMedium();
		break;
		case hard:
			dispDifHard();
		break;
		default:
		break;
	}
}
void dispDifEasy(){
	// EASY highlighted in the top left
	
	unsigned char selE[8] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selA[8] = {0x1f, 0x1b, 0x15, 0x11, 0x15, 0x15, 0x1f, 0x00};
	unsigned char selS[8] = {0x1f, 0x11, 0x17, 0x11, 0x1d, 0x11, 0x1f, 0x00};	
	unsigned char selY[8] = {0x1f, 0x15, 0x15, 0x1b, 0x1b, 0x1b, 0x1f, 0x00};	
		
	LCD_Custom_Char(0, selE);		// build at position 0...
	LCD_Custom_Char(1, selA);
	LCD_Custom_Char(2, selS);
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('M');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('D');
	LCD_Cursor(cur++);
	LCD_WriteData('I');
	LCD_Cursor(cur++);
	LCD_WriteData('U');
	LCD_Cursor(cur++);
	LCD_WriteData('M');
	LCD_Cursor(cur++);		// Newline here
	LCD_WriteData('H');
	LCD_Cursor(cur++);
	LCD_WriteData('A');
	LCD_Cursor(cur++);
	LCD_WriteData('R');
	LCD_Cursor(cur++);
	LCD_WriteData('D');
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
}
void dispDifMedium(){
	// Medium highlighted in the top right
	
	unsigned char selE[8] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selM[8] = {0x1f, 0x15, 0x11, 0x11, 0x15, 0x15, 0x1f, 0x00};
	unsigned char selD[8] = {0x1f, 0x13, 0x15, 0x15, 0x15, 0x13, 0x1f, 0x00};
	unsigned char selI[8] = {0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x11, 0x1f, 0x00};	
	unsigned char selU[8] = {0x1f, 0x15, 0x15, 0x15, 0x15, 0x11, 0x1f, 0x00};	
	
	LCD_Custom_Char(0, selM);		// build at position 0...
	LCD_Custom_Char(1, selE);
	LCD_Custom_Char(2, selD);
	LCD_Custom_Char(3, selI);
	LCD_Custom_Char(4, selU);
	
	unsigned char cur = 1;		// initialize cursor to 1
	
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('A');
	LCD_Cursor(cur++);
	LCD_WriteData('S');
	LCD_Cursor(cur++);
	LCD_WriteData('Y');
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(0);
	LCD_Cursor(cur++);
	LCD_WriteData(1);
	LCD_Cursor(cur++);
	LCD_WriteData(2);
	LCD_Cursor(cur++);
	LCD_WriteData(3);
	LCD_Cursor(cur++);
	LCD_WriteData(4);
	LCD_Cursor(cur++);
	LCD_WriteData(0);
	LCD_Cursor(cur++);		// Newline here
	LCD_WriteData('H');
	LCD_Cursor(cur++);
	LCD_WriteData('A');
	LCD_Cursor(cur++);
	LCD_WriteData('R');
	LCD_Cursor(cur++);
	LCD_WriteData('D');
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
}
void dispDifHard(){
	// Medium highlighted in the top right
	
	unsigned char selH[8] = {0x1f, 0x15, 0x15, 0x11, 0x15, 0x15, 0x1f, 0x00};
	unsigned char selA[8] = {0x1f, 0x1b, 0x15, 0x11, 0x15, 0x15, 0x1f, 0x00};	
	unsigned char selD[8] = {0x1f, 0x13, 0x15, 0x15, 0x15, 0x13, 0x1f, 0x00};
	unsigned char selR[8] = {0x1f, 0x13, 0x15, 0x13, 0x15, 0x15, 0x1f, 0x00};	
	
	LCD_Custom_Char(0, selH);		// build at position 0...
	LCD_Custom_Char(1, selA);
	LCD_Custom_Char(2, selR);
	LCD_Custom_Char(3, selD);
	
	unsigned char cur = 1;		// initialize cursor to 1
	
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('A');
	LCD_Cursor(cur++);
	LCD_WriteData('S');
	LCD_Cursor(cur++);
	LCD_WriteData('Y');
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData('M');
	LCD_Cursor(cur++);
	LCD_WriteData('E');
	LCD_Cursor(cur++);
	LCD_WriteData('D');
	LCD_Cursor(cur++);
	LCD_WriteData('I');
	LCD_Cursor(cur++);
	LCD_WriteData('U');
	LCD_Cursor(cur++);
	LCD_WriteData('M');
	LCD_Cursor(cur++);		// Newline here
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
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
	LCD_Cursor(cur++);
	LCD_WriteData(' ');
}

void LCD_Custom_Char (unsigned char loc, unsigned char *msg){
	int i;
	LCD_WriteCommand (0x40 + (loc*8));	/* Command 0x40 for CGRAM */
	for(i = 0;i < 8; i++)	/* 8 cause 8 lines x 5 rows per character */
		LCD_WriteData(msg[i]);
	LCD_WriteCommand(0x80);
}

void dispTopRight(){
	// DIFFICULTY highlighted in the top right
	
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
void dispBottomRight(){
	// 2PLAYER highlighted in the bottom right
	
	unsigned char sel2[8] = {0x1f, 0x13, 0x1d, 0x1d, 0x1b, 0x11, 0x1f, 0x00};
	unsigned char selP[8] = {0x1f, 0x11, 0x15, 0x11, 0x17, 0x17, 0x1f, 0x00};
	unsigned char selL[8] = {0x1f, 0x17, 0x17, 0x17, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selA[8] = {0x1f, 0x1b, 0x15, 0x11, 0x15, 0x15, 0x1f, 0x00};
	unsigned char selY[8] = {0x1f, 0x15, 0x15, 0x1b, 0x1b, 0x1b, 0x1f, 0x00};
	unsigned char selE[8] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selR[8] = {0x1f, 0x13, 0x15, 0x13, 0x15, 0x15, 0x1f, 0x00};
	
	LCD_Custom_Char(0, sel2);		// build T at position 0...
	LCD_Custom_Char(1, selP);
	LCD_Custom_Char(2, selL);
	LCD_Custom_Char(3, selA);
	LCD_Custom_Char(4, selY);
	LCD_Custom_Char(5, selE);
	LCD_Custom_Char(6, selR);
	
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
	LCD_WriteData(0);
	LCD_Cursor(cur++);
	LCD_WriteData(1);
	LCD_Cursor(cur++);
	LCD_WriteData(2);
	LCD_Cursor(cur++);
	LCD_WriteData(3);
	LCD_Cursor(cur++);
	LCD_WriteData(4);
	LCD_Cursor(cur++);
	LCD_WriteData(5);
	LCD_Cursor(cur++);
	LCD_WriteData(6);
}
void dispBottomLeft(){
	// TIME highlighted in the bottom left
	
	unsigned char selE[8] = {0x1f, 0x11, 0x17, 0x13, 0x17, 0x11, 0x1f, 0x00};
	unsigned char selM[8] = {0x1f, 0x15, 0x11, 0x11, 0x15, 0x15, 0x1f, 0x00};
	unsigned char selI[8] = {0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x11, 0x1f, 0x00};
	unsigned char selT[8] = {0x1f, 0x11, 0x1b, 0x1b, 0x1b, 0x1b, 0x1f, 0x00};
	
	LCD_Custom_Char(0, selT);		// build T at position 0...
	LCD_Custom_Char(1, selI);
	LCD_Custom_Char(2, selM);
	LCD_Custom_Char(3, selE);
	
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
	// PINA: 4 select
	
	
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
			else if(!GetBit(PINA, 4)){	// select "play"
				while(!GetBit(PINA, 4));
				inMenu = 0;
				inGame = 1;
			}
			else{
				mm = topLeft;
			}
		break;
		case topRight:
			if(!GetBit(PINA, 3)){	// left
				mm = topLeft;
			}
			else if(!GetBit(PINA, 2)){	// down
				mm = bottomRight;
			}
			else if(!GetBit(PINA, 4)){	// select 'difficulty'
				while(!GetBit(PINA, 4));	// wait for release
				inMenu = 0;	// no longer in menu
				inDifficulty = 1;	// in difficulty menu
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
			else if(!GetBit(PINA, 4)){
				while(!GetBit(PINA, 4));
				inMenu = 0;
				inTime = 1;
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
			mm = topLeft;
		break;
	}
	switch(mm){			// actions
		case init:
			dispTopLeft();
		break;
		case topLeft:
			dispTopLeft();
		break;
		case topRight:
			dispTopRight();
		break;
		case bottomLeft:
			dispBottomLeft();
		break;
		case bottomRight:
			dispBottomRight();
		break;
		default:
			dispTopLeft();
		break;
	}
}

void SNES_init() {
	SNES_PORT |= (0x01 << CLOCK);
	SNES_PORT |= (0x01 << LATCH);
}
unsigned char readSNES(){
	
	unsigned char data = 0x00;

	SNES_PORT |= (0x01 << LATCH);
	SNES_PORT &= ~(0x01 << LATCH);
	// B, Y, SELECT, START, UP, DOWN, LEFT, RIGHT, A, X, L, R, NA, NA, NA, NA
	
	data = ((SNES_PIN & (0x01 << DATA)) >> DATA);

	for(int i = 0; i > 16; i++){
		SNES_PORT &= ~(0x01 << CLOCK);
		data = data << 1;
		data = data | ((SNES_PIN & (0x01 << DATA)) >> DATA);
		SNES_PORT |= (0x01 << CLOCK);
		PORTC = SNES_PIN;
	}
	

	return data;
}
void eraseEEPROM(){
	unsigned char erase = 999;
	eeprom_write_word(&eeprom_array[0], erase);	// 
	eeprom_write_word(&eeprom_array[1], erase);	// 
	gameTime = erase;
	difficulty = erase;
}

void playGame(){
	nokia_lcd_ingame();
}

int main() {
	DDRA = 0x00; PORTA = 0xFF; // A input
	DDRB = 0xFF; PORTB = 0x00; // B output
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines (output)

	LCD_init();
	nokia_lcd_init();
	SNES_init();
	
	LCD_WriteCommand(0x38); /* function set */
	LCD_WriteCommand(0x0c); /* display on,cursor off,blink off */
	
	//nokia_lcd_write_string("Hello World!", 1);
	//nokia_lcd_render();
	
	unsigned char timerTime = 1;
	
	TimerSet(timerTime);	// until we start blinking lights, no need to set timer just yet
	TimerOn(); 
	
	//eeprom_write_word(&eeprom_array[0], difficulty);	// default to difficulty medium
	//eeprom_write_word(&eeprom_array[1], gameTime);	// default to 60 seconds
	
	difficulty = eeprom_read_word(&eeprom_array[0]);
	gameTime = eeprom_read_word(&eeprom_array[1]);
	
	mm = init;
	diff = initDif;
	time_s = initTime;
	
	unsigned short period1second = 12.8;	// speed up - go down. Slow down - go up
	int i = 0;
	unsigned char secPassed = 0;
	
	LCD_ClearScreen();
	while(1){	
		nokia_lcd_menu(gameTime, difficulty);		// sets the Nokia LCD to display time and difficulty
		if(inMenu){
			displayMenu();
		}
		if(inDifficulty){
			setDifficulty();
		}
		if(inTime){
			setTime();
		}
		while(inGame){
			playGame();
		}
		unsigned char controller = ~readSNES();		// B press = reset EEPROM
		if(controller & 0x01 == 0x01){
			eraseEEPROM();
		}
		/*
		if(i >= period1second){
			secPassed++;
			nokia_lcd_timer(secPassed);
			//nokia_lcd_write_string("Hello World!", 1);
			nokia_lcd_render();
			i = 0;
		}
		else
		i += timerTime;
		*/
		while(!TimerFlag);
		TimerFlag = 0;
	}
}


