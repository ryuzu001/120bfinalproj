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

enum mainMenu{init, topLeft, topRight, bottomLeft, bottomRight} mm;
enum difficultyMenu{initDif, easy, medium, hard} diff;
	
unsigned char difficultyEasy = 1;
unsigned char difficultyMedium = 0;	// default to medium
unsigned char difficultyHard = 0;

unsigned char inMenu = 1;
unsigned char inDifficulty = 0;

void displayMenu();
void dispTopLeft();
void dispTopRight();
void dispBottomLeft();
void dispBottomRight();
void LCD_Custom_Char (unsigned char loc, unsigned char *msg);
/*
void topLeftSelect();
void topRightSelect();
void bottomLeftSelect();
void bottomRightSelect(); */
void setDifficulty();
void dispDifEasy();
void dispDifMedium();
void dispDifHard();

void setDifficulty(){
	// PINA: 0 up
	// PINA: 1 right
	// PINA: 2 down
	// PINA: 3 left
	// PINA: 4 select

	switch(diff){	// transitions
		case initDif:
			if(difficultyEasy)
			diff = easy;
			else if(difficultyMedium)
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
				difficultyEasy = 1;		// set difficulty to easy
				difficultyMedium = 0;
				difficultyHard = 0;
				
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
				difficultyEasy = 0;
				difficultyMedium = 1;
				difficultyHard = 0;
				
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
				difficultyEasy = 0;
				difficultyMedium = 0;
				difficultyHard = 1;
				
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
			if(difficultyEasy){
				dispDifEasy();
			}
			else if(difficultyMedium){
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
			if(difficultyEasy){
				dispDifEasy();
			}
			else if(difficultyMedium){
				dispDifMedium();
			}
			else{
				dispDifHard();
			}
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
	for(i = 0;i < 8; i++)	/* 7 cause the bottom is reserved for the cursor */
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

int main() {
	DDRA = 0x00; PORTA = 0xFF; // A input
	DDRB = 0xFF; PORTB = 0x00; // B output
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines (output)

	LCD_init();
	
	LCD_WriteCommand(0x38); /* function set */
	LCD_WriteCommand(0x0c); /* display on,cursor off,blink off */
	TimerSet(1);	// until we start blinking lights, no need to set timer just yet
	TimerOn(); 
	
	mm = init;
	diff = initDif;
	
	LCD_ClearScreen();
	while(1){	
		if(inMenu){
			displayMenu();
		}
		if(inDifficulty){
			setDifficulty();
		}
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
}


