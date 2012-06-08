/*
 * spi.c
 *
 *  Created on: Dec 31, 2011
 *      Author: hammer
 */

#include <string.h>
#include "lpc_2148.h"
#include "util.h"
#include "lcd.h"
#include "jni.h"

#define SPI_OK          0           // transfer ended No Errors
#define SPI_BUSY        1           // transfer busy
#define SPI_ERROR       2           // SPI error
static unsigned char state; // State of SPI driver
//static unsigned char spiBuf[4]; // SPI data buffer
static unsigned char *msg; // pointer to SPI data buffer
static unsigned char count; // nr of bytes send/received


#define SELECT IOCLR0 = 0x00000080
//#define SELECT IOSET0 = 0x00000080

#define DESELECT IOSET0 = 0x00000080
//#define DESELECT IOCLR0 = 0x00000080

void SPI_Isr(void) __attribute__ ((interrupt));

void SPI_Isr(void) //__irq
{
	if ((S0SPSR & 0xF8) == 0x80) {
		*msg++ = S0SPDR; // read byte from slave
		if (--count > 0)
			S0SPDR = *msg; // sent next byte
		else
			state = SPI_OK; // transfer completed
	} else // SPI error
	{
		*msg = S0SPDR; // dummy read to clear flags
		state = SPI_ERROR;
	}
	S0SPINT = 0x01; // reset interrupt flag
	VICVectAddr = 0; // reset VIC
}

static void SPI_Init(void) {
	VICVectAddr0 = (unsigned int) &SPI_Isr;
	VICVectCntl0 = 0x2A; // Channel0 on Source#10 ... enabled
	VICIntEnable |= 0x400; // 10th bit is the SPI

	IODIR0 |= 0x00000080; // P0.7 defined as SS_DS1722
	DESELECT;
	PINSEL0 |= 0x00001500; // configure SPI0 pins (except SSEL0)
	// According to http://excamera.com/sphinx/gameduino/porting.html the Gameduino is
	// able to run 8 MHz.
	//S0SPCCR = 12; // SCK = 1 MHz, counter > 8 and even
	//S0SPCCR = 60; // SCK = 1 MHz, counter > 8 and even
	//S0SPCCR = 14; // Maybe use 14 for gameduino (4.5 MHz; 14 => 4.29 MHz)
	S0SPCCR = 8; // 60MHz / 8 = 7.5 MHz and even.
	S0SPCR = 0x20; // CPHA=0, CPOL=0, master mode, MSB first, interrupt disabled
}

#define bit_is_clear(sfr, bit) (!(sfr & (1<<bit)))
/*
 static void SPI_Write(size_t bytes, char tx[]) {
 char timeout; //Completion Status
 char junk; //Dummy Variable to clear Rx Buffer
 size_t i; //Local Byte Counter
 int j; //Timeout Failure Counter


 SELECT;

 if (bytes) {
 timeout = 0;
 for (i = 0; ((!timeout) && (i < bytes)); i++) {
 junk = S0SPSR; //Clear Transfer Complete Flag
 junk = S0SPDR; //Clear RX buffer
 S0SPDR = tx[i]; //Transmit Character(s)

 for (j = 0; ((j < 1000000) && bit_is_clear(S0SPSR, 7)); j++) {
 };
 if (bit_is_clear(S0SPSR, 7)) {
 timeout = 1;
 }
 }

 } else {
 timeout = 1;
 }

 DESELECT;
 }
 */
//static char buf[20];
static int maxTimeout = 0;
static unsigned short SPI_transfer(unsigned short c) {
	//char timeout; //Completion Status
	int j; //Timeout Failure Counter

	S0SPDR = c & 0xff; // Transmit byte

	for (j = 0; ((j < 1000000) && bit_is_clear(S0SPSR, 7)); j++) {
	};
	if (bit_is_clear(S0SPSR, 7)) {
		//		timeout = 1;
	}
	if (j > maxTimeout) {
		maxTimeout = j;
	}

	// Return data from SPI:
	return S0SPDR;
}

static unsigned short SPI_read(int addr) {
	unsigned short byteRead;

	SELECT;
	SPI_transfer((addr & 0xff00) >> 8);
	SPI_transfer(addr & 0xff);
	byteRead = SPI_transfer(0);
	DESELECT;

	return byteRead;
}

/**
 * This method fills a char array with data from the gameduinos memory
 * \param addr The address to read from
 * \param bytes The char array to write in
 * \param length The number of bytes to read
 */
static void SPI_read_bytes(int addr, char* bytes, size_t length) {
	SELECT;
	SPI_transfer((addr & 0xff00) >> 8);
	SPI_transfer(addr & 0xff);
	while (length-- > 0) {
		*bytes++ = SPI_transfer(0);
	}
	DESELECT;
}

/**
 * This method fills the gameduinos memory with data from the byte array
 * \param addr The address to write to
 * \param bytes The char array to read from
 * \param length The number of bytes to write
 */
static void SPI_write_bytes(int addr, char* bytes, size_t length) {
	SELECT;
	SPI_transfer(((addr & 0xff00) >> 8) | 0x80);
	SPI_transfer(addr & 0xff);
	while (length-- > 0) {
		SPI_transfer(*bytes++);
	}
	DESELECT;
}

static void SPI_write(int addr, unsigned int c) {
	SELECT;
	SPI_transfer(((addr & 0xff00) >> 8) | 0x80);
	SPI_transfer(addr & 0xff);
	SPI_transfer(c);
	DESELECT;
}

static void SPI_write16(int addr, unsigned int val) {
	SELECT;
	SPI_transfer(((addr & 0xff00) >> 8) | 0x80);
	SPI_transfer(addr & 0xff);
	SPI_transfer(val & 0xff);
	SPI_transfer((val >> 8) & 0xff);
	DESELECT;
}

static void SPI_fill(int addr, unsigned short value, size_t count) {
	SELECT;
	SPI_transfer(((addr & 0xff00) >> 8) | 0x80);
	SPI_transfer(addr & 0xff);
	while (count-- > 0) {
		SPI_transfer(value);
	}
	DESELECT;
}

#define IDENT         0x2800

int gameduino_init(void) {
	int status;
	delay(250000);
	SPI_Init();

	int i = 0;
	while (i < 0x1000) {
		SPI_write(i++, 'T');
		SPI_write(i++, 'h');
		SPI_write(i++, 'i');
		SPI_write(i++, 'n');
		SPI_write(i++, 'J');
		SPI_write(i++, ' ');
	}

	if (SPI_read(IDENT) == 0x6d) {
		// Gameduino identification read successfully:
		status = 0;
	} else {
		// Gameduino identification could not be read; is it connected?
		status = -1;
	}

	return status;
}

void JNICALL Java_thinj_gameduino_Gameduino_writeByte(JNIEnv *env, jclass jc, jint address,
		jint val) {
	SPI_write((int) address, (unsigned int) val);
	//int i = 0;
	//	while (i < 0x1000) {
	//		SPI_write(i++, ' ');
	//	}
	//	SPI_write(8, '0' + (addr % 10)); addr /= 10;
	//	SPI_write(7, '0' + (addr % 10)); addr /= 10;
	//	SPI_write(6, '0' + (addr % 10)); addr /= 10;
	//	SPI_write(5, '0' + (addr % 10)); addr /= 10;
	//	SPI_write(4, '0' + (addr % 10)); addr /= 10;
	//	SPI_write(3, '0' + (addr % 10)); addr /= 10;
	//	SPI_write(2, '0' + (addr % 10)); addr /= 10;
	//	SPI_write(1, '0' + (addr % 10)); addr /= 10;
	//	SPI_write(0, '0' + (addr % 10)); addr /= 10;
}

jint JNICALL Java_thinj_gameduino_Gameduino_readByte(JNIEnv *env, jclass jc, jint address) {
	return SPI_read((int) address);
}

JNIEXPORT void JNICALL Java_thinj_gameduino_Gameduino_writeShort(JNIEnv *env, jclass jc, jint addr,
		jint val) {
	SPI_write16((int) addr, (unsigned int) val);
}

void JNICALL Java_thinj_gameduino_Gameduino_readBytes(JNIEnv *env, jclass jc, jint address,
		jobject byteArray) {
	OBJECT_TO_S1_ARRAY(data, len, byteArray);

	SPI_read_bytes(address, data, len);
}

void JNICALL Java_thinj_gameduino_Gameduino_writeBytes(JNIEnv *env, jclass jc, jint address,
		jobject byteArray) {
	OBJECT_TO_S1_ARRAY(data, len, byteArray);

	SPI_write_bytes(address, data, len);
}


void JNICALL Java_thinj_gameduino_Gameduino_fill(JNIEnv *env, jclass jc, jint address, jint value, jint length) {
	SPI_fill((int)address, (unsigned short) value, (size_t) length);
}
