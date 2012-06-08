/*
 * config.h
 *
 *  Created on: Jun 23, 2011
 *      Author: hammer
 */

#ifndef CONFIG_H_
#define CONFIG_H_


//insert heap and stack size in this file

//#define HEAP_SIZE 2000
//#define STACK_SIZE 100
#define HEAP_SIZE 9000
#define STACK_SIZE 1000


//#define USE_DEBUG

#ifdef USE_DEBUG
#define DEB(X) X
#define jvmexit(X) exit(X)
#else
#include "console.h"
#define DEB(X)
#define jvmexit(X) do {consout("jvmexit %s %d\n", __FILE__, __LINE__); while(1);} while (0)
#endif

#define PUTCHAR(x) rs232_putchar(x)

// We have no stdlib linked in:
#define NO_STD_LIB

#endif /* CONFIG_H_ */
