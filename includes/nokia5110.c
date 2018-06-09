/* Nokia 5110 LCD AVR Library
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 */

#include "nokia5110.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>
#include "nokia5110_chars.h"


static struct {
    /* screen byte massive */
    uint8_t screen[504];

    /* cursor position */
    uint8_t cursor_x;
    uint8_t cursor_y;

} nokia_lcd = {
    .cursor_x = 0,
    .cursor_y = 0
};

/**
 * Sending data to LCD
 * @bytes: data
 * @is_data: transfer mode: 1 - data; 0 - command;
 */
static void write(uint8_t bytes, uint8_t is_data)
{
	register uint8_t i;
	/* Enable controller */
	PORT_LCD &= ~(1 << LCD_SCE);

	/* We are sending data */
	if (is_data)
		PORT_LCD |= (1 << LCD_DC);
	/* We are sending commands */
	else
		PORT_LCD &= ~(1 << LCD_DC);

	/* Send bytes */
	for (i = 0; i < 8; i++) {
		/* Set data pin to byte state */
		if ((bytes >> (7-i)) & 0x01)
			PORT_LCD |= (1 << LCD_DIN);
		else
			PORT_LCD &= ~(1 << LCD_DIN);

		/* Blink clock */
		PORT_LCD |= (1 << LCD_CLK);
		PORT_LCD &= ~(1 << LCD_CLK);
	}

	/* Disable controller */
	PORT_LCD |= (1 << LCD_SCE);
}

static void write_cmd(uint8_t cmd)
{
	write(cmd, 0);
}

static void write_data(uint8_t data)
{
	write(data, 1);
}

/*
 * Public functions
 */

void nokia_lcd_init(void)
{
	register unsigned i;
	/* Set pins as output */
	DDR_LCD |= (1 << LCD_SCE);
	DDR_LCD |= (1 << LCD_RST);
	DDR_LCD |= (1 << LCD_DC);
	DDR_LCD |= (1 << LCD_DIN);
	DDR_LCD |= (1 << LCD_CLK);

	/* Reset display */
	PORT_LCD |= (1 << LCD_RST);
	PORT_LCD |= (1 << LCD_SCE);
	_delay_ms(10);
	PORT_LCD &= ~(1 << LCD_RST);
	_delay_ms(70);
	PORT_LCD |= (1 << LCD_RST);

	/*
	 * Initialize display
	 */
	/* Enable controller */
	PORT_LCD &= ~(1 << LCD_SCE);
	/* -LCD Extended Commands mode- */
	write_cmd(0x21);
	/* LCD bias mode 1:48 */
	write_cmd(0x13);
	/* Set temperature coefficient */
	write_cmd(0x06);
	/* Default VOP (3.06 + 66 * 0.06 = 7V) */
	write_cmd(0xC2);
	/* Standard Commands mode, powered down */
	write_cmd(0x20);
	/* LCD in normal mode */
	write_cmd(0x09);

	/* Clear LCD RAM */
	write_cmd(0x80);
	write_cmd(LCD_CONTRAST);
	for (i = 0; i < 504; i++)
		write_data(0x00);

	/* Activate LCD */
	write_cmd(0x08);
	write_cmd(0x0C);
}

void nokia_lcd_clear(void)
{
	register unsigned i;
	/* Set column and row to 0 */
	write_cmd(0x80);
	write_cmd(0x40);
	/*Cursor too */
	nokia_lcd.cursor_x = 0;
	nokia_lcd.cursor_y = 0;
	/* Clear everything (504 bytes = 84cols * 48 rows / 8 bits) */
	for(i = 0;i < 504; i++)
		nokia_lcd.screen[i] = 0x00;
}

void nokia_lcd_power(uint8_t on)
{
	write_cmd(on ? 0x20 : 0x24);
}

void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value)
{
	uint8_t *byte = &nokia_lcd.screen[y/8*84+x];
	if (value)
		*byte |= (1 << (y % 8));
	else
		*byte &= ~(1 << (y %8 ));
}

void nokia_lcd_write_char(char code, uint8_t scale)
{
	register uint8_t x, y;

	for (x = 0; x < 5*scale; x++)
		for (y = 0; y < 7*scale; y++)
			if (pgm_read_byte(&CHARSET[code-32][x/scale]) & (1 << y/scale))
				nokia_lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 1);
			else
				nokia_lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 0);

	nokia_lcd.cursor_x += 5*scale + 1;
	if (nokia_lcd.cursor_x >= 84) {
		nokia_lcd.cursor_x = 0;
		nokia_lcd.cursor_y += 7*scale + 1;
	}
	if (nokia_lcd.cursor_y >= 48) {
		nokia_lcd.cursor_x = 0;
		nokia_lcd.cursor_y = 0;
	}
}

void nokia_lcd_write_string(const char *str, uint8_t scale)
{
	while(*str)
		nokia_lcd_write_char(*str++, scale);
}

void nokia_lcd_set_cursor(uint8_t x, uint8_t y)
{
	nokia_lcd.cursor_x = x;
	nokia_lcd.cursor_y = y;
}

void nokia_lcd_render(void)
{
	register unsigned i;
	/* Set column and row to 0 */
	write_cmd(0x80);
	write_cmd(0x40);

	/* Write screen to display */
	for (i = 0; i < 504; i++)
		write_data(nokia_lcd.screen[i]);
}

void nokia_lcd_custom(void){
	nokia_lcd_write_char(0x7f, 1);
}

void nokia_lcd_menu(unsigned char num, unsigned char d){
	nokia_lcd_clear();
	if(num == 30){
		nokia_lcd_write_char(0x33, 1);
		nokia_lcd_write_char(0x30, 1);
		nokia_lcd_write_string(" seconds", 1);	// [gametime] seconds
	}
	else if(num == 60){
		nokia_lcd_write_char(0x36, 1);
		nokia_lcd_write_char(0x30, 1);
		nokia_lcd_write_string(" seconds", 1);	// [gametime] seconds
	}
	else if(num == 90){
		nokia_lcd_write_char(0x39, 1);
		nokia_lcd_write_char(0x30, 1);
		nokia_lcd_write_string(" seconds", 1);	// [gametime] seconds
	}
	nokia_lcd_set_cursor(0, 10);			// newline
	if(d == 0){
		nokia_lcd_write_string("Diff: Easy", 1);
	}
	else if(d == 1){
		nokia_lcd_write_string("Diff: Medium", 1);
	}
	else if(d == 2){
		nokia_lcd_write_string("Diff: Hard", 1);
	}
	nokia_lcd_set_cursor(0, 20);			// newline
	nokia_lcd_write_string("Time set: ", 1);
	if(num == 30 || num == 60 || num == 90){
		nokia_lcd_custom();
	}
	nokia_lcd_set_cursor(0, 30);
	nokia_lcd_write_string("Diff set: ", 1);
	if(d == 0 || d == 1 || d == 2){
		nokia_lcd_custom();
	}
	nokia_lcd_render();
}

unsigned char score = 0;		// GAME SCORE

void nokia_lcd_nottrying(){
	nokia_lcd_clear();
	nokia_lcd_write_string("Let's not go below 0", 1);
	nokia_lcd_set_cursor(0, 20);
	nokia_lcd_write_string(":(", 1);
	nokia_lcd_set_cursor(0, 30);
	nokia_lcd_write_string("Setting score to 0", 1);
	nokia_lcd_render();
	delay_ms(3000);
	score = 0;
}

void nokia_lcd_ingame(char score, char secPassed){
	nokia_lcd_clear();
	nokia_lcd_write_string("Score: ", 1);
	/*
	if(score == -1)
	nokia_lcd_write_string("-1", 1);
	else if(score == -2)
	nokia_lcd_write_string("-2", 1);
	else if(score == -3)
	nokia_lcd_write_string("-3", 1);
	else if(score == -4)
	nokia_lcd_write_string("-4", 1);
	else if(score == -5)
	nokia_lcd_write_string("-5", 1);
	else if(score == -6)
	nokia_lcd_write_string("-6", 1);
	else if(score == -7)
	nokia_lcd_write_string("-7", 1);
	else if(score == -8)
	nokia_lcd_write_string("-8", 1);
	else if(score == -9)
	nokia_lcd_write_string("-9", 1);
	else if(score == -10)
	nokia_lcd_write_string("-10", 1);
	*/
	
	if(score == 0)
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
	else{ 
		score = 0;
		nokia_lcd_nottrying();
	}
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("  +  ", 1);
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string("*   -", 1);
	nokia_lcd_set_cursor(0, 30);
	nokia_lcd_write_string("  /  ", 1);
	nokia_lcd_set_cursor(0, 40);
	nokia_lcd_write_string("Time : ", 1);
	if(secPassed == 0)
	nokia_lcd_write_string("0", 1);
	else if(secPassed == 1)
	nokia_lcd_write_string("1", 1);
	else if(secPassed == 2)
	nokia_lcd_write_string("2", 1);
	else if(secPassed == 3)
	nokia_lcd_write_string("3", 1);
	else if(secPassed == 4)
	nokia_lcd_write_string("4", 1);
	else if(secPassed == 5)
	nokia_lcd_write_string("5", 1);
	else if(secPassed == 6)
	nokia_lcd_write_string("6", 1);
	else if(secPassed == 7)
	nokia_lcd_write_string("7", 1);
	else if(secPassed == 8)
	nokia_lcd_write_string("8", 1);
	else if(secPassed == 9)
	nokia_lcd_write_string("9", 1);
	else if(secPassed == 10)
	nokia_lcd_write_string("10", 1);
	else if(secPassed == 11)
	nokia_lcd_write_string("11", 1);
	else if(secPassed == 12)
	nokia_lcd_write_string("12", 1);
	else if(secPassed == 13)
	nokia_lcd_write_string("13", 1);
	else if(secPassed == 14)
	nokia_lcd_write_string("14", 1);
	else if(secPassed == 15)
	nokia_lcd_write_string("15", 1);
	else if(secPassed == 16)
	nokia_lcd_write_string("16", 1);
	else if(secPassed == 17)
	nokia_lcd_write_string("17", 1);
	else if(secPassed == 18)
	nokia_lcd_write_string("18", 1);
	else if(secPassed == 19)
	nokia_lcd_write_string("19", 1);
	else if(secPassed == 20)
	nokia_lcd_write_string("20", 1);
	else if(secPassed == 21)
	nokia_lcd_write_string("21", 1);
	else if(secPassed == 22)
	nokia_lcd_write_string("22", 1);
	else if(secPassed == 23)
	nokia_lcd_write_string("23", 1);
	else if(secPassed == 24)
	nokia_lcd_write_string("24", 1);
	else if(secPassed == 25)
	nokia_lcd_write_string("25", 1);
	else if(secPassed == 26)
	nokia_lcd_write_string("26", 1);
	else if(secPassed == 27)
	nokia_lcd_write_string("27", 1);
	else if(secPassed == 28)
	nokia_lcd_write_string("28", 1);
	else if(secPassed == 29)
	nokia_lcd_write_string("29", 1);
	else if(secPassed == 30)
	nokia_lcd_write_string("30", 1);
	else if(secPassed == 31)
	nokia_lcd_write_string("31", 1);
	else if(secPassed == 32)
	nokia_lcd_write_string("32", 1);
	else if(secPassed == 33)
	nokia_lcd_write_string("33", 1);
	else if(secPassed == 34)
	nokia_lcd_write_string("34", 1);
	else if(secPassed == 35)
	nokia_lcd_write_string("35", 1);
	else if(secPassed == 36)
	nokia_lcd_write_string("36", 1);
	else if(secPassed == 37)
	nokia_lcd_write_string("37", 1);
	else if(secPassed == 38)
	nokia_lcd_write_string("38", 1);
	else if(secPassed == 39)
	nokia_lcd_write_string("39", 1);
	else if(secPassed == 40)
	nokia_lcd_write_string("40", 1);
	else if(secPassed == 41)
	nokia_lcd_write_string("41", 1);
	else if(secPassed == 42)
	nokia_lcd_write_string("42", 1);
	else if(secPassed == 43)
	nokia_lcd_write_string("43", 1);
	else if(secPassed == 44)
	nokia_lcd_write_string("44", 1);
	else if(secPassed == 45)
	nokia_lcd_write_string("45", 1);
	else if(secPassed == 46)
	nokia_lcd_write_string("46", 1);
	else if(secPassed == 47)
	nokia_lcd_write_string("47", 1);
	else if(secPassed == 48)
	nokia_lcd_write_string("48", 1);
	else if(secPassed == 49)
	nokia_lcd_write_string("49", 1);
	else if(secPassed == 50)
	nokia_lcd_write_string("50", 1);
	else if(secPassed == 51)
	nokia_lcd_write_string("51", 1);
	else if(secPassed == 52)
	nokia_lcd_write_string("52", 1);
	else if(secPassed == 53)
	nokia_lcd_write_string("53", 1);
	else if(secPassed == 54)
	nokia_lcd_write_string("54", 1);
	else if(secPassed == 55)
	nokia_lcd_write_string("55", 1);
	else if(secPassed == 56)
	nokia_lcd_write_string("56", 1);
	else if(secPassed == 57)
	nokia_lcd_write_string("57", 1);
	else if(secPassed == 58)
	nokia_lcd_write_string("58", 1);
	else if(secPassed == 59)
	nokia_lcd_write_string("59", 1);
	else if(secPassed == 60)
	nokia_lcd_write_string("60", 1);
	else if(secPassed == 61)
	nokia_lcd_write_string("61", 1);
	else if(secPassed == 62)
	nokia_lcd_write_string("62", 1);
	else if(secPassed == 63)
	nokia_lcd_write_string("63", 1);
	else if(secPassed == 64)
	nokia_lcd_write_string("64", 1);
	else if(secPassed == 65)
	nokia_lcd_write_string("65", 1);
	else if(secPassed == 66)
	nokia_lcd_write_string("66", 1);
	else if(secPassed == 67)
	nokia_lcd_write_string("67", 1);
	else if(secPassed == 68)
	nokia_lcd_write_string("68", 1);
	else if(secPassed == 69)
	nokia_lcd_write_string("69", 1);
	else if(secPassed == 70)
	nokia_lcd_write_string("70", 1);
	else if(secPassed == 71)
	nokia_lcd_write_string("71", 1);
	else if(secPassed == 72)
	nokia_lcd_write_string("72", 1);
	else if(secPassed == 73)
	nokia_lcd_write_string("73", 1);
	else if(secPassed == 74)
	nokia_lcd_write_string("74", 1);
	else if(secPassed == 75)
	nokia_lcd_write_string("75", 1);
	else if(secPassed == 76)
	nokia_lcd_write_string("76", 1);
	else if(secPassed == 77)
	nokia_lcd_write_string("77", 1);
	else if(secPassed == 78)
	nokia_lcd_write_string("78", 1);
	else if(secPassed == 79)
	nokia_lcd_write_string("79", 1);
	else if(secPassed == 80)
	nokia_lcd_write_string("80", 1);
	else if(secPassed == 81)
	nokia_lcd_write_string("81", 1);
	else if(secPassed == 82)
	nokia_lcd_write_string("82", 1);
	else if(secPassed == 83)
	nokia_lcd_write_string("83", 1);
	else if(secPassed == 84)
	nokia_lcd_write_string("84", 1);
	else if(secPassed == 85)
	nokia_lcd_write_string("85", 1);
	else if(secPassed == 86)
	nokia_lcd_write_string("86", 1);
	else if(secPassed == 87)
	nokia_lcd_write_string("87", 1);
	else if(secPassed == 88)
	nokia_lcd_write_string("88", 1);
	else if(secPassed == 89)
	nokia_lcd_write_string("89", 1);
	else if(secPassed == 90)
	nokia_lcd_write_string("90", 1);
	
	
	nokia_lcd_render();
}