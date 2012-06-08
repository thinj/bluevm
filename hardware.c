/*
 * hardware.c
 *
 *  Created on: Oct 28, 2011
 *      Author: hammer
 */

#include "config.h"
#include "hardware.h"
#include "rs232.h"
#include "types.h"
#include "gameduino.h"

// TODO consider where to prototype these

void buzz(BOOL on);
void delay_ms(jint delayInMillis);

void init_hardware(void) {
	initPLL();

	buzz(TRUE);

	rs232_init();
	rs232_puts("*******************************");
	rs232_puts("Welcome to thinJ on LPC2148 ARM");
	rs232_puts("*******************************");

	delay_ms(100);
	buzz(FALSE);
	if (gameduino_init() != 0) {
		rs232_puts("Gameduino not available");
	}
}

