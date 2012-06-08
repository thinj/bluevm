/*
 * rs232.h
 *
 *  Created on: Oct 28, 2011
 *      Author: hammer
 */

#ifndef RS232_H_
#define RS232_H_

/**
 * Initializes UART0 for 38400 baud etc.
 */
void rs232_init(void);

/**
 * Blocks until a byte is received
 * \return The received byte
 */
char rs232_getchar(void);

/**
 * Transmits a byte on UART0. Blocks until the transmit register is empty; then the byte is send
 * \param c The byte to send
 */
void rs232_putchar(char c);

/**
 * Transmits a string of characters on UART0
 * \param cp The string to send. The string is terminated using '\n'
 */
void rs232_puts(char *cp);


#endif /* RS232_H_ */
