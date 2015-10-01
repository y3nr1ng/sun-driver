/*
   $Id:$

   ST7565 LCD library!

   Copyright (C) 2010 Limor Fried, Adafruit Industries

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

   // some of this code was written by <cstone@pobox.com> originally; it is in the public domain.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "stlcd.h"
#include "glcd.h"
#include "util.h"
#include "icon.h"


#define BLA_DDR DDRB
#define BLA_PIN PINB
#define BLA_PORT PORTB
#define BLA 2

#define SID_DDR DDRB
#define SID_PIN PINB
#define SID_PORT PORTB
#define SID 1

#define SCLK_DDR DDRB
#define SCLK_PIN PINB
#define SCLK_PORT PORTB
#define SCLK 0

#define A0_DDR DDRD
#define A0_PIN PIND
#define A0_PORT PORTD
#define A0 7

#define RST_DDR DDRD
#define RST_PIN PIND
#define RST_PORT PORTD
#define RST 6

#define CS_DDR DDRD
#define CS_PIN PIND
#define CS_PORT PORTD
#define CS 5

#define LED_DDR DDRD
#define LED_PORT PORTD
#define LED 2

uint8_t is_reversed = 0;

int pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

uint8_t buffer[128*64/8] = {
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,

	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x3, 0x7, 0xF, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x7, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x7F, 0x3F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x1F, 0x3F, 0x70, 0x70, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x6, 0x0, 0x0, 0x0, 0x3, 0x3,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,

	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0xF, 0x7, 0x7,
	0x7, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF, 0x3F,
	0x70, 0x60, 0x60, 0x60, 0x60, 0x30, 0x7F, 0x3F, 0x0, 0x0, 0x1F, 0x3F, 0x70, 0x60, 0x60, 0x60,
	0x60, 0x39, 0xFF, 0xFF, 0x0, 0x6, 0x1F, 0x39, 0x60, 0x60, 0x60, 0x60, 0x30, 0x3F, 0x7F, 0x0,
	0x0, 0x60, 0xFF, 0xFF, 0x60, 0x60, 0x0, 0x7F, 0x7F, 0x70, 0x60, 0x60, 0x40, 0x0, 0x7F, 0x7F,
	0x0, 0x0, 0x0, 0x0, 0x7F, 0x7F, 0x0, 0x0, 0x0, 0x7F, 0x7F, 0x0, 0x0, 0x60, 0xFF, 0xFF,
	0x60, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,

	0x80, 0xF8, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xE7, 0xE7, 0xE3,
	0xF3, 0xF9, 0xFF, 0xFF, 0xFF, 0xF7, 0x7, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x3F, 0x3F, 0x1F, 0xF, 0x7, 0x3, 0x0, 0x0, 0x0, 0xC0,
	0xE0, 0x60, 0x20, 0x20, 0x60, 0xE0, 0xE0, 0xE0, 0x0, 0x0, 0x80, 0xC0, 0xE0, 0x60, 0x20, 0x60,
	0x60, 0xE0, 0xE0, 0xE0, 0x0, 0x0, 0x80, 0xC0, 0x60, 0x60, 0x20, 0x60, 0x60, 0xE0, 0xE0, 0x0,
	0x0, 0x0, 0xE0, 0xE0, 0x0, 0x0, 0x0, 0xE0, 0xE0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0xE0,
	0x60, 0x60, 0x60, 0x60, 0xE0, 0x80, 0x0, 0x0, 0x0, 0xE0, 0xE0, 0x0, 0x0, 0x0, 0xE0, 0xE0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,

	0x0, 0x0, 0x0, 0x3, 0x7, 0x1F, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF1, 0xE3,
	0xE3, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFC, 0x7F, 0x3F, 0x3F, 0x3F, 0x3F, 0x7F, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF0, 0xE0, 0x80, 0x0, 0x0, 0x0, 0xC,
	0x1C, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7F, 0x7F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x7, 0x7, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1C, 0xC, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,

	0x0, 0x7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFE, 0xFC, 0xF8,
	0xF8, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xFF,
	0xFF, 0x0, 0x0, 0x0, 0xFF, 0xFF, 0xE0, 0xC0, 0xC0, 0xC0, 0xFF, 0x7F, 0x0, 0x0, 0x1E, 0x7F,
	0xE1, 0xC0, 0xC0, 0xC0, 0xC0, 0x61, 0xFF, 0xFF, 0x0, 0x0, 0xFE, 0xFF, 0x1, 0x0, 0x0, 0x0,
	0xFF, 0xFF, 0x0, 0x0, 0x21, 0xF9, 0xF8, 0xDC, 0xCC, 0xCF, 0x7, 0x0, 0xC0, 0xFF, 0xFF, 0xC0,
	0x80, 0x0, 0xFF, 0xFF, 0xC0, 0xC0, 0x80, 0x0, 0x0, 0xFF, 0xFF, 0x0, 0x0, 0x1F, 0x7F, 0xF9,
	0xC8, 0xC8, 0xC8, 0xC8, 0x79, 0x39, 0x0, 0x0, 0x71, 0xF9, 0xD8, 0xCC, 0xCE, 0x47, 0x3, 0x0,

	0x0, 0x0, 0x0, 0x0, 0x80, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xF8, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF0, 0xC0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC0,
	0xC0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x80,
	0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x80, 0xC0, 0xC0, 0xC0, 0xC0,
	0xC0, 0x80, 0x0, 0x0, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0x0,
	0x0, 0x0, 0xC0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC0, 0xC0, 0x0, 0x0, 0x0, 0x80, 0xC0,
	0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x80, 0x0, 0x0, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0x80, 0x0, 0x0,

	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,

};

int main(void) {
	setup();
	while (1)
	{
		loop();
	}
}

void loop(void) {
}

void setup(void) {

	//Serial.begin(9600);
	uart_init(BRRL_192);

	// turn on backlight
	BLA_DDR |= _BV(BLA);
	BLA_PORT |= _BV(BLA);

	LED_DDR |= _BV(LED);
	// Serial.println("init");
	putstring_nl("init!");

	st7565_init();


	/*
	   while (1) {
	   spiwrite(0x55);
	   _delay_ms(100);
	   }
	 */

	//Serial.println("on");
	putstring_nl("on");
	st7565_command(CMD_DISPLAY_ON);
	//Serial.println("normal");
	putstring_nl("normal");
	st7565_command(CMD_SET_ALLPTS_NORMAL);
	//Serial.println("bright");
	putstring_nl("bright");
	st7565_set_brightness(0x18);
	//Serial.println("clear");
	putstring_nl("clear");
	clear_screen();
	//Serial.println("blit");
	putstring_nl("blit");

	write_buffer(buffer);
	putstring_nl("done");
	_delay_ms(250);
	clear_buffer(buffer);

	LED_PORT |= _BV(LED);

	//drawrect(buffer, 10, 10, 2, 2, 1);
	// testdrawrect(buffer);
	// testfillrect(buffer);
	//  testdrawline(buffer);
	//testdrawcircle(buffer);
	//fillcircle(buffer, 32, 32, 10, 1);
	//testdrawchar(buffer);
	// drawstring(buffer, 0, 0, "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
	testdrawbitmap(buffer, logo16_glcd_bmp, 16, 16);

	LED_PORT &= ~_BV(LED);
	write_buffer(buffer);
}

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

void testdrawbitmap(uint8_t *buff, const uint8_t *bitmap, uint8_t w, uint8_t h) {
	uint8_t icons[NUMFLAKES][3];
	srandom(buff[666]);

	// initialize
	for (uint8_t f=0; f< NUMFLAKES; f++)
	{
		icons[f][XPOS] = random() % 128;
		icons[f][YPOS] = 0;
		icons[f][DELTAY] = random() % 5 + 1;
	}

	while (1)
	{
		// draw each icon
		for (uint8_t f=0; f< NUMFLAKES; f++)
		{
			drawbitmap(buffer, icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, 1);
		}
		write_buffer(buffer);
		_delay_ms(200);

		// then erase it + move it
		for (uint8_t f=0; f< NUMFLAKES; f++)
		{
			drawbitmap(buffer, icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, 0);
			// move it
			icons[f][YPOS] += icons[f][DELTAY];
			// if its gone, reinit
			if (icons[f][YPOS] > 64) {
				icons[f][XPOS] = random() % 128;
				icons[f][YPOS] = 0;
				icons[f][DELTAY] = random() % 5 + 1;
			}
		}
	}
}

void testdrawchar(uint8_t *buff) {
	for (uint8_t i=0; i < 168; i++)
	{
		drawchar(buffer, (i % 21) * 6, i/21, i);
	}
}

void testdrawcircle(uint8_t *buff) {
	for (uint8_t i=0; i<64; i+=2)
	{
		drawcircle(buffer, 63, 31, i, 1);
	}
}

void testdrawline(uint8_t *buff) {
	for (uint8_t i=0; i<128; i+=4)
	{
		drawline(buffer, 0, 0, i, 63, 1);
	}
	for (uint8_t i=0; i<64; i+=4)
	{
		drawline(buffer, 0, 0, 127, i, 1);
	}

	write_buffer(buffer);
	_delay_ms(1000);

	for (uint8_t i=0; i<128; i+=4)
	{
		drawline(buffer, i, 63, 0, 0, 0);
	}
	for (uint8_t i=0; i<64; i+=4)
	{
		drawline(buffer, 127, i, 0, 0, 0);
	}
}

void testdrawrect(uint8_t *buff) {
	for (uint8_t i=0; i<64; i+=2)
	{
		drawrect(buff, i, i, 128-i, 64-i, 1);

	}
}


void testfillrect(uint8_t *buff) {
	for (uint8_t i=0; i<64; i++)
	{
		fillrect(buff, i, i, 128-i, 64-i, i%2);

	}
}

void clear_screen(void) {
	uint8_t p, c;

	for (p = 0; p < 8; p++)
	{
		/*
		   putstring("new page! ");
		   uart_putw_dec(p);
		   putstring_nl("");
		 */

		st7565_command(CMD_SET_PAGE | p);
		for (c = 0; c < 129; c++)
		{
			//uart_putw_dec(c);
			//uart_putchar(' ');
			st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
			st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
			st7565_data(0x0);
		}
	}
}


void st7565_init(void) {
	// set pin directions
	SID_DDR |= _BV(SID);
	SCLK_DDR |= _BV(SCLK);
	A0_DDR |= _BV(A0);
	RST_DDR |= _BV(RST);
	CS_DDR |= _BV(CS);

	// toggle RST low to reset; CS low so it'll listen to us
	CS_PORT &= ~_BV(CS);
	RST_PORT &= ~_BV(RST);
	_delay_ms(500);
	RST_PORT |= _BV(RST);

	// LCD bias select
	st7565_command(CMD_SET_BIAS_7);
	// ADC select
	st7565_command(CMD_SET_ADC_NORMAL);
	// SHL select
	st7565_command(CMD_SET_COM_NORMAL);
	// Initial display line
	st7565_command(CMD_SET_DISP_START_LINE);

	// turn on voltage converter (VC=1, VR=0, VF=0)
	st7565_command(CMD_SET_POWER_CONTROL | 0x4);
	// wait for 50% rising
	_delay_ms(50);

	// turn on voltage regulator (VC=1, VR=1, VF=0)
	st7565_command(CMD_SET_POWER_CONTROL | 0x6);
	// wait >=50ms
	_delay_ms(50);

	// turn on voltage follower (VC=1, VR=1, VF=1)
	st7565_command(CMD_SET_POWER_CONTROL | 0x7);
	// wait
	_delay_ms(10);

	// set lcd operating voltage (regulator resistor, ref voltage resistor)
	st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);

	// initial display line
	// set page address
	// set column address
	// write display data
}

inline void spiwrite(uint8_t c) {
	int8_t i;
	for (i=7; i>=0; i--)
	{
		SCLK_PORT &= ~_BV(SCLK);
		if (c & _BV(i))
			SID_PORT |= _BV(SID);
		else
			SID_PORT &= ~_BV(SID);
		SCLK_PORT |= _BV(SCLK);
	}

	/*
	   // loop unwrapped! too fast doesnt work :(

	   SCLK_PORT &= ~_BV(SCLK);
	   if (c & _BV(7))
	   SID_PORT |= _BV(SID);
	   else
	   SID_PORT &= ~_BV(SID);
	   SCLK_PORT |= _BV(SCLK);

	   SCLK_PORT &= ~_BV(SCLK);
	   if (c & _BV(6))
	   SID_PORT |= _BV(SID);
	   else
	   SID_PORT &= ~_BV(SID);
	   SCLK_PORT |= _BV(SCLK);

	   SCLK_PORT &= ~_BV(SCLK);
	   if (c & _BV(5))
	   SID_PORT |= _BV(SID);
	   else
	   SID_PORT &= ~_BV(SID);
	   SCLK_PORT |= _BV(SCLK);

	   SCLK_PORT &= ~_BV(SCLK);
	   if (c & _BV(4))
	   SID_PORT |= _BV(SID);
	   else
	   SID_PORT &= ~_BV(SID);
	   SCLK_PORT |= _BV(SCLK);

	   SCLK_PORT &= ~_BV(SCLK);
	   if (c & _BV(3))
	   SID_PORT |= _BV(SID);
	   else
	   SID_PORT &= ~_BV(SID);
	   SCLK_PORT |= _BV(SCLK);

	   SCLK_PORT &= ~_BV(SCLK);
	   if (c & _BV(2))
	   SID_PORT |= _BV(SID);
	   else
	   SID_PORT &= ~_BV(SID);
	   SCLK_PORT |= _BV(SCLK);


	   SCLK_PORT &= ~_BV(SCLK);
	   if (c & _BV(1))
	   SID_PORT |= _BV(SID);
	   else
	   SID_PORT &= ~_BV(SID);
	   SCLK_PORT |= _BV(SCLK);

	   SCLK_PORT &= ~_BV(SCLK);
	   if (c & _BV(0))
	   SID_PORT |= _BV(SID);
	   else
	   SID_PORT &= ~_BV(SID);
	   SCLK_PORT |= _BV(SCLK);
	 */

}
void st7565_command(uint8_t c) {
	A0_PORT &= ~_BV(A0);

	spiwrite(c);
}

void st7565_data(uint8_t c) {
	A0_PORT |= _BV(A0);

	spiwrite(c);
}
void st7565_set_brightness(uint8_t val) {
	st7565_command(CMD_SET_VOLUME_FIRST);
	st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}


void write_buffer(uint8_t *buffer) {
	uint8_t c, p;

	for (p = 0; p < 8; p++)
	{
		/*
		   putstring("new page! ");
		   uart_putw_dec(p);
		   putstring_nl("");
		 */

		st7565_command(CMD_SET_PAGE | pagemap[p]);
		st7565_command(CMD_SET_COLUMN_LOWER | (0x0 & 0xf));
		st7565_command(CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));
		st7565_command(CMD_RMW);
		st7565_data(0xff);

		//st7565_data(0x80);
		//continue;

		for (c = 0; c < 128; c++)
		{
			//uart_putw_dec(c);
			//uart_putchar(' ');
			st7565_data(buffer[(128*p)+c]);
		}
	}
}
