/*	Ruifeng Zhang rzhan016@ucr.edu
 *	Ryan Yuzuki ryuzu001@ucr.edu
 *	Lab Section: 024
 *	Assignment: Lab #10  Exercise #2
 *	Exercise Description: Buttons are connected to PA0 and PA1. Output for PORTC 
 *  is initially 7. Pressing PA0 increments PORTC once (stopping at 9). Pressing
 *  PA1 decrements PORTC once (stopping at 0). If both buttons are depressed 
 *  (even if not initially simultaneously), PORTC resets to 0. USING LCD SCREEN
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

int main() {
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();
	
	LCD_ClearScreen();
	
	
	LCD_DisplayString(1, hello);
}


