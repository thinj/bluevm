/*
 * util.c
 *
 *  Created on: Aug 12, 2011
 *      Author: hammer
 */

#include "lpc_2148.h"
#include "util.h"

void delay(long micro)
{
	long l;
	micro = 5*micro;
	for (l = 0; l < micro; l++) {
		//__asm__("mov r0,r0");
	}
}

void usb_blink(void) {
	// set P0.31 to output:
	IO0DIR |= (1 << 31);
	while (1) {
		// Turn off USB LED:
		IO0CLR |= (1 << 31);
		delay(500);
		// Turn on USB LED:
		IO0SET |= (1 << 31);
		delay(575 * 1000 - 500);
	}
}
