/*
 * mystdlib.h
 *
 *  Created on: Jan 14, 2012
 *      Author: hammer
 */

#ifndef MYSTDLIB_H_
#define MYSTDLIB_H_

#include  <stdlib.h>
#include "config.h"

/**
 * This method is replacement for memcpy.
 * \param void* s1 The pointer to the destination
 * \param void* s2 The pointer to the source
 * \param n The number of bytes to copy
 */
void *mymemcpy(void *s1, const void * s2, size_t n);

/**
 * This function writes len bytes of value c (converted to an unsigned char) to the byte string b.
 * This is replacement for memset.
 * \param void* s1 The pointer to the destination
 * \param c The byte to set
 * \param len The number of bytes to set
 *
 */
void *mymemset(void *s1, int c, size_t len);

/**
 * This method is replacement for strlen
 * \param s The string to calculate length of
 * \return The number of bytes pointed to be s excluding trailing '\0'
 */
size_t mystrlen(const char *s);


#endif /* MYSTDLIB_H_ */
