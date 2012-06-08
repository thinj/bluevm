/*
 * rs232.c
 *
 *  Created on: Oct 23, 2011
 *      Author: hammer
 */

#include "lpc_2148.h"
#include "rs232.h"

static void port_init(void);

void rs232_init(void) {
	port_init();
}

void rs232_puts(char *cp) {
	while (*cp != '\0') {
		rs232_putchar(*cp++);
	}
	rs232_putchar('\n');
}

char rs232_getchar(void) {
	while (!(U0LSR && 0x01))
		;
	return U0RBR;
}

void rs232_putchar(char c) {
	while (!(U0LSR && 0x20))
		;
	U0THR = c;
}

static void port_init(void) {
	PINSEL0 = 0x00000005; //enable TXD0/RXD0 pins - P0.0 & 1
	U0LCR = 0x83; //8-N-1, enable DLAB
	//	U0DLM = 0; //115200 baud
	//	U0DLL = 0x13;
	U0DLM = 0; //38400 baud
	U0DLL = 57;//0x39;
	U0FCR = 0x07; //enable & clear FIFOs
	U0FDR = 0x00000075; //set fractional divider
	U0LCR = 0x03; //disable DLAB

	delay(10000);
    //	16 x (256 x U0DLM + U0DLL)x(1+5/7) = 1562.5
	//	16 x (U0DLL)x(1+5/7) = 1562.5
	//	(U0DLL)x(1+5/7) = 1562.5/16 = 97,65625
	//	U0DLL = 97,65625 / (1+5/7) = 7 * 97,65625 / 12 = 56.97 ~ 57
}

//char rs232_getchar(void) {
//	while (!(U1LSR && 0x01))
//		;
//	return U1RBR;
//}
//
//void rs232_putchar(char c) {
//	while (!(U1LSR && 0x20))
//		;
//	U1THR = c;
//}
//
//static void port_init(void) {
//	//PINSEL0 = 0x00000005; //enable TXD0/RXD0 pins - P0.0 & 1
//	PINSEL0 = 0x00000010; //enable TXD0/RXD0 pins - P0.8 & 9
//	U1LCR = 0x83; //8-N-1, enable DLAB
//	//	U0DLM = 0; //115200 baud
//	//	U0DLL = 0x13;
//	U1DLM = 0; //38400 baud
//	U1DLL = 57;//0x39;
//	U1FCR = 0x07; //enable & clear FIFOs
//	U1FDR = 0x00000075; //set fractional divider
//	U1LCR = 0x03; //disable DLAB
//
//	delay(10000);
//    //	16 x (256 x U0DLM + U0DLL)x(1+5/7) = 1562.5
//	//	16 x (U0DLL)x(1+5/7) = 1562.5
//	//	(U0DLL)x(1+5/7) = 1562.5/16 = 97,65625
//	//	U0DLL = 97,65625 / (1+5/7) = 7 * 97,65625 / 12 = 56.97 ~ 57
//}
