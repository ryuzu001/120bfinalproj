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
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io.c"
#include "bit.h"
#include "timer.h"
#include "nokia5110.c"
#include "questions.c"

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

struct questionEasy{
	char n1[3];
	char n2[3];
	char ans[3];
	int qid;
};
struct questionMedium{
	int n1;
	int n2;
	int n3;
	int ans;
	int qid;
};
struct questionHard{
	int n1;
	int n2;
	int n3;
	int n4;
	int ans;
	int qid;
};
	
unsigned char difficulty = 1;	// diffculty - 0 easy, 1 medium 2 hard. default to medium.
unsigned char gameTime = 60;	// gametime in seconds

time_t t;

unsigned int totalGameTime = 0;
//unsigned short period1second = 12.8;	// speed up - go down. Slow down - go up
unsigned short period1second = 19;
int timeRemaining = 0;
int secPassed = 0;

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
	
	score = 0;
	
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

void error(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Please set a difficulty and time!", 1);
	nokia_lcd_render();
	delay_ms(2000);
}
void endgame(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Game Over!", 1);
	nokia_lcd_set_cursor(0, 10);
	nokia_lcd_write_string("You scored ", 1);
	if(score <= 0)
	nokia_lcd_write_string("0", 1);
	else if(score == 1)
	nokia_lcd_write_string("1", 1);
	else if(score == 2)
	nokia_lcd_write_string("2", 1);
	else if(score == 3)
	nokia_lcd_write_string("3", 1);
	else if(score == 4)
	nokia_lcd_write_string("4", 1);
	else if(score == 5)
	nokia_lcd_write_string("5", 1);
	else if(score == 6)
	nokia_lcd_write_string("6", 1);
	else if(score == 7)
	nokia_lcd_write_string("7", 1);
	else if(score == 8)
	nokia_lcd_write_string("8", 1);
	else if(score == 9)
	nokia_lcd_write_string("9", 1);
	else if(score == 10)
	nokia_lcd_write_string("10", 1);
	else if(score == 11)
	nokia_lcd_write_string("11", 1);
	else if(score == 12)
	nokia_lcd_write_string("12", 1);
	else if(score == 13)
	nokia_lcd_write_string("13", 1);
	else if(score == 14)
	nokia_lcd_write_string("14", 1);
	else if(score == 15)
	nokia_lcd_write_string("15", 1);
	else if(score == 16)
	nokia_lcd_write_string("16", 1);
	else if(score == 17)
	nokia_lcd_write_string("17", 1);
	else if(score == 18)
	nokia_lcd_write_string("18", 1);
	else if(score == 19)
	nokia_lcd_write_string("19", 1);
	else if(score == 20)
	nokia_lcd_write_string("20", 1);
	else if(score == 21)
	nokia_lcd_write_string("21", 1);
	else if(score == 22)
	nokia_lcd_write_string("22", 1);
	else if(score == 23)
	nokia_lcd_write_string("23", 1);
	else if(score == 24)
	nokia_lcd_write_string("24", 1);
	else if(score == 25)
	nokia_lcd_write_string("25", 1);
	else if(score == 26)
	nokia_lcd_write_string("26", 1);
	else if(score == 27)
	nokia_lcd_write_string("27", 1);
	else if(score == 28)
	nokia_lcd_write_string("28", 1);
	else if(score == 29)
	nokia_lcd_write_string("29", 1);
	else if(score == 30)
	nokia_lcd_write_string("30", 1);
	nokia_lcd_set_cursor(0, 20);
	nokia_lcd_write_string("Nice!", 1);
	
	nokia_lcd_set_cursor(0, 30);
	nokia_lcd_write_string("Returning to main menu...", 1);
	nokia_lcd_render();
	delay_ms(5000);
}

void playQuestionEasy(struct questionEasy q){
	
	char str[50];
	char str2[50];
	
	strcpy(str, q.n1);
	strcat(str, " ");
	strcat(str, q.n2);
	strcat(str, "=");
	strcat(str, q.ans);
	strcat(str, "            ");
	
	strcpy(str2, " ");
	
	LCD_ClearScreen();
	LCD_Cursor(1);
	int i, str2length;
	str2length = 1;
	for(i = 0; i < 15; i++) {
		LCD_WriteData(str[i]);
	}
	LCD_Cursor(17);
	unsigned char questionCorrect = 0x00;
	// PINA: 0 up
	// PINA: 1 right
	// PINA: 2 down
	// PINA: 3 left
	// PINA: 4 select
	while (str2length < 2)
	{
		if(!GetBit(PINA, 0)){
			while(!GetBit(PINA, 0));
			if(q.qid == 0 || q.qid == 6
			|| q.qid == 7 || q.qid == 11
			|| q.qid == 19 || q.qid == 20)
				questionCorrect++;
			strcat(str2, "+ ");
			str2length++;
		}
		else if(!GetBit(PINA, 1)){
			while(!GetBit(PINA, 1));
			if(q.qid == 3 || q.qid == 4
			|| q.qid == 12 || q.qid == 17
			|| q.qid == 18 || q.qid == 7)
				questionCorrect++;
			strcat(str2, "- ");
			str2length++;
		}
		else if(!GetBit(PINA, 2)){
			while(!GetBit(PINA, 2));
			if(q.qid == 2 || q.qid == 10
			|| q.qid == 13 || q.qid == 14
			|| q.qid == 15 || q.qid == 16)
				questionCorrect++;
			strcat(str2, "/ ");
			str2length++;
		}
		else if(!GetBit(PINA, 3)){
			while(!GetBit(PINA, 3));
			if(q.qid == 1 || q.qid == 5
			|| q.qid == 8 || q.qid == 9)
				questionCorrect++;
			strcat(str2, "* ");
			str2length++;
		}
		if(timeRemaining >= period1second){
			secPassed++;
			timeRemaining = 0;
		}
		else
		timeRemaining++;
		nokia_lcd_ingame(score, gameTime-secPassed);
		//delay_ms(500);		// change this to not mess with timing
		if(secPassed >= gameTime){
			endgame();
			inGame = 0;
			inMenu = 1;
			break;
		}
	}
	for(i = 0; i < str2length; i++) {
		LCD_WriteData(str2[i]);
	}
	if(questionCorrect == 0x01){
		score++;
	}
	else{
		score--;
	}
}

void playQuestionMedium(struct questionMedium q){
	
	char str[50];
	char str2[50];
	char an[50];	// answer string, will hold " +-" or something of that sort
	
	strcpy(str, q.n1);
	strcat(str, " ");
	strcat(str, q.n2);
	strcat(str, " ");
	strcat(str, q.n3);
	strcat(str, "=");
	strcat(str, q.ans);
	strcat(str, "            ");
	
	strcpy(str2, " ");
	strcpy(an, " ");
	
	LCD_ClearScreen();
	LCD_Cursor(1);
	int i, str2length;
	str2length = 1;
	for(i = 0; i < 15; i++) {
		LCD_WriteData(str[i]);
	}
	LCD_Cursor(17);
	// PINA: 0 up
	// PINA: 1 right
	// PINA: 2 down
	// PINA: 3 left
	// PINA: 4 select
	while (str2length < 3)
	{
		if(!GetBit(PINA, 0)){
			while(!GetBit(PINA, 0));
			strcat(an, "+");
			strcat(str2, "+ ");
			str2length++;
		}
		else if(!GetBit(PINA, 1)){
			while(!GetBit(PINA, 1));
			strcat(an, "-");
			strcat(str2, "- ");
			str2length++;
		}
		else if(!GetBit(PINA, 2)){
			while(!GetBit(PINA, 2));
			strcat(an, "/");
			strcat(str2, "/ ");
			str2length++;
		}
		else if(!GetBit(PINA, 3)){
			while(!GetBit(PINA, 3));
			strcat(an, "*");
			strcat(str2, "* ");
			str2length++;
		}
		if(timeRemaining >= period1second){
			secPassed++;
			timeRemaining = 0;
		}
		else
		timeRemaining++;
		nokia_lcd_ingame(score, gameTime-secPassed);
		//delay_ms(500);		// change this to not mess with timing
		LCD_Cursor(17);
		for(i = 0; i < str2length; i++) {
			LCD_WriteData(str2[i]);
		}
		if(secPassed >= gameTime){
			endgame();
			inGame = 0;
			inMenu = 1;
			break;
		}
	}
		
		// Check answers here - question id's vs input 
		if(q.qid == 0){
			if(an[1] == '+' && an[2] == '+'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 1){
			if(an[1] == '+' && an[2] == '*'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 2){
			if(an[1] == '*' && an[2] == '+'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 3){
			if(an[1] == '*' && an[2] == '-'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 4){
			if(an[1] == '-' && an[2] == '/'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 5){
			if(an[1] == '/' && an[2] == '+'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 6){
			if(an[1] == '+' && an[2] == '/'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 7){
			if(an[1] == '-' && an[2] == '+'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 8){
			if(an[1] == '*' && an[2] == '*'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 9){
			if(an[1] == '-' && an[2] == '*'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 10){
			if(an[1] == '/' && an[2] == '/'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 11){
			if(an[1] == '*' && an[2] == '+'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 12){
			if(an[1] == '+' && an[2] == '*'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 13){
			if(an[1] == '+' && an[2] == '/'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 14){
			if(an[1] == '*' && an[2] == '-'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 15){
			if(an[1] == '-' && an[2] == '/'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 16){
			if(an[1] == '/' && an[2] == '+'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 17){
			if(an[1] == '-' && an[2] == '*'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 18){
			if(an[1] == '*' && an[2] == '-'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 19){
			if(an[1] == '/' && an[2] == '*'){
				score++;
			}
			else
			score--;
		}
		if(q.qid == 20){
			if(an[1] == '/' && an[2] == '*'){
				score++;
			}
			else
			score--;
		}
}

void playQuestionHard(struct questionHard q){
	
	char str[50];
	char str2[50];
	char an[50];	// answer string, will hold " +-" or something of that sort
	
	strcpy(str, q.n1);
	strcat(str, " ");
	strcat(str, q.n2);
	strcat(str, " ");
	strcat(str, q.n3);
	strcat(str, " ");
	strcat(str, q.n4);
	strcat(str, "=");
	strcat(str, q.ans);
	strcat(str, "            ");
	
	strcpy(str2, " ");
	strcpy(an, " ");
	
	LCD_ClearScreen();
	LCD_Cursor(1);
	int i, str2length;
	str2length = 1;
	for(i = 0; i < 15; i++) {
		LCD_WriteData(str[i]);
	}
	LCD_Cursor(17);
	// PINA: 0 up
	// PINA: 1 right
	// PINA: 2 down
	// PINA: 3 left
	// PINA: 4 select
	unsigned char cur = 17;
	while (str2length < 4)
	{
		if(!GetBit(PINA, 0)){
			while(!GetBit(PINA, 0));
			strcat(an, "+");
			strcat(str2, "+ ");
			str2length++;
		}
		else if(!GetBit(PINA, 1)){
			while(!GetBit(PINA, 1));
			strcat(an, "-");
			strcat(str2, "- ");
			str2length++;
		}
		else if(!GetBit(PINA, 2)){
			while(!GetBit(PINA, 2));
			strcat(an, "/");
			strcat(str2, "/ ");
			str2length++;
		}
		else if(!GetBit(PINA, 3)){
			while(!GetBit(PINA, 3));
			strcat(an, "*");
			strcat(str2, "* ");
			str2length++;
		}
		if(timeRemaining >= period1second){
			secPassed++;
			timeRemaining = 0;
		}
		else
		timeRemaining++;
		nokia_lcd_ingame(score, gameTime-secPassed);
		//delay_ms(500);		// change this to not mess with timing
		LCD_DisplayStringNoErase(17, str2);
		if(secPassed >= gameTime){
			endgame();
			inGame = 0;
			inMenu = 1;
			break;
		}
	}
	
	// Check answers here - question id's vs input
	if(q.qid == 0){
		if(an[1] == '*' && an[2] == '/' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 1){
		if(an[1] == '-' && an[2] == '*' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 2){
		if(an[1] == '*' && an[2] == '/' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 3){
		if(an[1] == '+' && an[2] == '+' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 4){
		if(an[1] == '+' && an[2] == '*' && an[3] == '-'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 5){
		if(an[1] == '*' && an[2] == '+' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 6){
		if(an[1] == '*' && an[2] == '-' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 7){
		if(an[1] == '-' && an[2] == '-' && an[3] == '-'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 8){
		if(an[1] == '-' && an[2] == '+' && an[3] == '*'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 9){
		if(an[1] == '+' && an[2] == '+' && an[3] == '-'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 10){
		if(an[1] == '*' && an[2] == '*' && an[3] == '*'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 11){
		if(an[1] == '-' && an[2] == '*' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 12){
		if(an[1] == '+' && an[2] == '+' && an[3] == '/'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 13){
		if(an[1] == '-' && an[2] == '/' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 14){
		if(an[1] == '*' && an[2] == '/' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 15){
		if(an[1] == '+' && an[2] == '/' && an[3] == '-'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 16){
		if(an[1] == '-' && an[2] == '*' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 17){
		if(an[1] == '+' && an[2] == '/' && an[3] == '/'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 18){
		if(an[1] == '/' && an[2] == '+' && an[3] == '/'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 19){
		if(an[1] == '*' && an[2] == '+' && an[3] == '+'){
			score++;
		}
		else
		score--;
	}
	if(q.qid == 20){
		if(an[1] == '*' && an[2] == '/' && an[3] == '-'){
			score++;
		}
		else
		score--;
	}
}


void playGame(){
	
		// *(NUMBERS), answer, question ID
	secPassed = 0;	
		
	struct questionEasy q0 = {"0","1","1",0};		// 0 + 1 = 1
	struct questionEasy q1 = {"6","8","48",1};	// 6 * 8 = 48
	struct questionEasy q2 = {"40","4","10",2};	// 40 / 4 = 10
	struct questionEasy q3 = {"22","14","8",3};	// 22 - 14 = 8
	struct questionEasy q4 = {"13","7","6",4};	// 13 - 7 = 6
	struct questionEasy q5 = {"5","5","25",5};	// 5 * 5 = 25
	struct questionEasy q6 = {"2","3","5",6};		// 2 + 3 = 5
	struct questionEasy q7 = {"6","0","6",7};		// 6 + 0 = 6
	struct questionEasy q8 = {"6","0","0",8};		// 6 * 0 = 0
	struct questionEasy q9 = {"8","0","0",9};		// 8 * 0 = 0
	struct questionEasy q10 = {"50","5","10",10};	// 50 / 5 = 10
	struct questionEasy q11 = {"27","15","42",11};	// 27 + 15 = 42
	struct questionEasy q12 = {"12","8","4",12};		// 12 - 8 = 4
	struct questionEasy q13 = {"12","6","2",13};		// 12/6=2
	struct questionEasy q14 = {"12","3","4",14};		// 12/3=4
	struct questionEasy q15 = {"12","4","3",15};		// 12/4=3
	struct questionEasy q16 = {"33","3","11",16};		// 33/3=11
	struct questionEasy q17 = {"34","12","22",17};	// 34-12=22
	struct questionEasy q18 = {"20","7","13",18};		// 20-7 = 13
	struct questionEasy q19 = {"11","4","15",19};		// 11+4=15
	struct questionEasy q20 = {"7","52","59",20};		// 7 + 52=59
		
	struct questionMedium m0 = {"1","1","1","3",0};		//1+1+1=3
	struct questionMedium m1 = {"4","2","3","10",1};	//4+2*3=10
	struct questionMedium m2 = {"0","4","4","4",2};		//0*4+4=4
	struct questionMedium m3 = {"6","7","2","40",3};	//6*7-2=40
	struct questionMedium m4 = {"3","1","2","1",4};		//3-1/2=1
	struct questionMedium m5 = {"45","5","9","18",5};	//45/5+9=18
	struct questionMedium m6 = {"5","5","5","6",6};		// 5+5/5=6	
	struct questionMedium m7 = {"7","3","4","8",7};	// 7-3+4=8	
	struct questionMedium m8 = {"2","3","4","12",8};	//2*3*4=12
	struct questionMedium m9 = {"100","1","2","48",9};	//50-1*2=48	
	struct questionMedium m10 = {"40","2","2","10",10};	// 40/2/2=10
	struct questionMedium m11 = {"5","6","4","34",11};	//5*6+4
	struct questionMedium m12 = {"1","7","3","22",12};	//1+7*3
	struct questionMedium m13 = {"1","5","5","2",13};	//1+5/5=2
	struct questionMedium m14 = {"3","8","4","20",14};	//3*8-4=20
	struct questionMedium m15 = {"4","8","4","2",15};	//4-8/4=2	
	struct questionMedium m16 = {"6","2","8","11",16};	//6/2+8	
	struct questionMedium m17 = {"11","3","2","5",17};	//11-3*2=5		
	struct questionMedium m18 = {"5","5","3","22",18};	//5*5-3=22		
	struct questionMedium m19 = {"22","11","4","8",19};	//22/11*4=8		
	struct questionMedium m20 = {"7","7","6","6",20};	//7/7*6=6		
			
	struct questionHard h0 = {"1","8","4","2","4",0};	//1*8/4+2=4
	struct questionHard h1 = {"9","6","1","4","7",1};	//9-6*1+4=7
	struct questionHard h2 = {"8","4","16","2","4",2};	//8*4/16+2=4
	struct questionHard h3 = {"1","1","1","1","4",3};	//1+1+1+1=4
	struct questionHard h4 = {"4","5","1","2","7",4};	//4+5*1-2=7
	struct questionHard h5 = {"7","2","1","1","16",5};	//7*2+1+1=16
	struct questionHard h6 = {"5","5","12","3","16",6};	//5*5-12+3=16
	struct questionHard h7 = {"16","4","4","3","5",7};	//16-4-4-3=5
	struct questionHard h8 = {"5","2","3","3","12",8};	//5-2+3*3=12
	struct questionHard h9 = {"3","6","4","8","5",9};	//3+6+4-8=5
	struct questionHard h10 = {"5","2","1","2","20",10};//5*2*1*2=20
	struct questionHard h11 = {"20","15","1","2","7",11};	//20-15*1+2=7
	struct questionHard h12 = {"7","2","10","2","14",12};	//7+2+10/2=14
	struct questionHard h13 = {"8","2","2","5","12",13};	//8-2/2+5=12
	struct questionHard h14 = {"8","2","2","5","13",14};	//8*2/2+5=13
	struct questionHard h15 = {"13","6","2","6","10",15};	//13+6/2-6=10
	struct questionHard h16 = {"9","5","0","2","11",16};	//9-5*0+2=11
	struct questionHard h17 = {"4","4","2","2","5",17};		//4+4/2/2=5
	struct questionHard h18 = {"7","7","5","5","2",18};		//7/7+5/5=2																		
	struct questionHard h19 = {"3","2","6","1","13",19};	//3*2+6+1=13		
	struct questionHard h20 = {"4","5","2","2","8",20};		//4*5/2-2=8
			
	if(gameTime == 30 || gameTime == 60 || gameTime == 90)
	while(inGame){
		if(difficulty == 0){
			int r = rand() % 21;
			LCD_ClearScreen();
			switch(r){
			case 0:
				playQuestionEasy(q0);
				break;
			case 1:	
				playQuestionEasy(q1);
				break;
			case 2:
				playQuestionEasy(q2);
				break;
			case 3:	
				playQuestionEasy(q3);
				break;		
			case 4:	
				playQuestionEasy(q4);
				break;
			case 5:	
				playQuestionEasy(q5);
				break;	
			case 6:	
				playQuestionEasy(q6);
				break;	
			case 7:	
				playQuestionEasy(q7);
				break;		
			case 8:	
				playQuestionEasy(q8);
				break;	
			case 9:	
				playQuestionEasy(q9);
				break;	
			case 10:
				playQuestionEasy(q10);
				break;	
			case 11:
				playQuestionEasy(q11);
				break;
			case 12:
				playQuestionEasy(q12);
				break;
			case 13:
				playQuestionEasy(q13);
				break;
			case 14:
				playQuestionEasy(q14);
				break;
			case 15:
				playQuestionEasy(q15);
				break;
			case 16:
				playQuestionEasy(q16);
				break;
			case 17:
				playQuestionEasy(q17);
				break;
			case 18:
				playQuestionEasy(q18);
				break;
			case 19:
				playQuestionEasy(q19);
				break;
			case 20:
				playQuestionEasy(q20);
				break;
			}
		}
		else if(difficulty == 2){
			int r = rand() % 21;
			LCD_ClearScreen();
			switch(r){
				case 0:
				playQuestionHard(h0);
				break;
				case 1:
				playQuestionHard(h1);
				break;
				case 2:
				playQuestionHard(h2);
				break;
				case 3:
				playQuestionHard(h3);
				break;
				case 4:
				playQuestionHard(h4);
				break;
				case 5:
				playQuestionHard(h5);
				break;
				case 6:
				playQuestionHard(h6);
				break;
				case 7:
				playQuestionHard(h7);
				break;
				case 8:
				playQuestionHard(h8);
				break;
				case 9:
				playQuestionHard(h9);
				break;
				case 10:
				playQuestionHard(h10);
				break;
				case 11:
				playQuestionHard(h11);
				break;
				case 12:
				playQuestionHard(h12);
				break;
				case 13:
				playQuestionHard(h13);
				break;
				case 14:
				playQuestionHard(h14);
				break;
				case 15:
				playQuestionHard(h15);
				break;
				case 16:
				playQuestionHard(h16);
				break;
				case 17:
				playQuestionHard(h17);
				break;
				case 18:
				playQuestionHard(h18);
				break;
				case 19:
				playQuestionHard(h19);
				break;
				case 20:
				playQuestionHard(h20);
				break;
			}
		}
		else if(difficulty == 1){
			int r = rand() % 21;
			LCD_ClearScreen();
			switch(r){
				case 0:
				playQuestionMedium(m0);
				break;
				case 1:
				playQuestionMedium(m1);
				break;
				case 2:
				playQuestionMedium(m2);
				break;
				case 3:
				playQuestionMedium(m3);
				break;
				case 4:
				playQuestionMedium(m4);
				break;
				case 5:
				playQuestionMedium(m5);
				break;
				case 6:
				playQuestionMedium(m6);
				break;
				case 7:
				playQuestionMedium(m7);
				break;
				case 8:
				playQuestionMedium(m8);
				break;
				case 9:
				playQuestionMedium(m9);
				break;
				case 10:
				playQuestionMedium(m10);
				break;
				case 11:
				playQuestionMedium(m11);
				break;
				case 12:
				playQuestionMedium(m12);
				break;
				case 13:
				playQuestionMedium(m13);
				break;
				case 14:
				playQuestionMedium(m14);
				break;
				case 15:
				playQuestionMedium(m15);
				break;
				case 16:
				playQuestionMedium(m16);
				break;
				case 17:
				playQuestionMedium(m17);
				break;
				case 18:
				playQuestionMedium(m18);
				break;
				case 19:
				playQuestionMedium(m19);
				break;
				case 20:
				playQuestionMedium(m20);
				break;
			}
		}
		else{
			error();
			inGame = 0;
			inMenu = 1;
		}
	}
	else{
		error();
		inGame = 0;
		inMenu = 1;
	}
}

int main() {
	DDRA = 0x00; PORTA = 0xFF; // A input
	DDRB = 0xFF; PORTB = 0x00; // B output
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines (output)

	LCD_init();
	nokia_lcd_init();
	SNES_init();
	srand((unsigned) time(&t));
	
	LCD_WriteCommand(0x38); /* function set */
	LCD_WriteCommand(0x0c); /* display on,cursor off,blink off */
	
	//nokia_lcd_write_string("Hello World!", 1);
	//nokia_lcd_render();
	
	unsigned char timerTime = 1;
	
	//TimerSet(timerTime);	// until we start blinking lights, no need to set timer just yet
	//TimerOn(); 
	
	//eeprom_write_word(&eeprom_array[0], difficulty);	// default to difficulty medium
	//eeprom_write_word(&eeprom_array[1], gameTime);	// default to 60 seconds
	
	difficulty = eeprom_read_word(&eeprom_array[0]);
	gameTime = eeprom_read_word(&eeprom_array[1]);
	
	mm = init;
	diff = initDif;
	time_s = initTime;
	
	//unsigned short period1second = 12.8;	// speed up - go down. Slow down - go up
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
		if(inGame){
			playGame();
		}
		unsigned char controller = ~readSNES();		// B press = reset EEPROM
		if(controller & 0x01 == 0x01){
			eraseEEPROM();
		}

		//while(!TimerFlag);
		//TimerFlag = 0;
	}
}


