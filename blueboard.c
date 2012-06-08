
/*
 * blueboard.c
 *
 *  Created on: Jun 22, 2011
 *      Author: hammer
 */

#include "jni.h"
#include "lcd.h"
#include "util.h"
#include "types.h"
#include "heap.h"
#include "rs232.h"

#include "lpc_2148.h"

// Copied from /workspace/LPC2148/lpc2148_demo/sysdefs.h
typedef unsigned char U8;
typedef unsigned int U32;

typedef volatile U8 REG8;
typedef volatile U32 REG32;

#define pREG8  (REG8 *)
#define pREG16 (REG16 *)
#define pREG32 (REG32 *)

// Copied from /workspace/LPC2148/lpc2148_demo/lpc214x.h

#define SCB_PLLCFG          (*(pREG32 (0xe01fc084)))
#define SCB_PLLCFG_MSEL     (0x0000001f)
#define SCB_PLLCFG_PSEL     (0x00000060)
#define SCB_PLLCFG_MUL1     (0x00000000)
#define SCB_PLLCFG_MUL2     (0x00000001)
#define SCB_PLLCFG_MUL3     (0x00000002)
#define SCB_PLLCFG_MUL4     (0x00000003)
#define SCB_PLLCFG_MUL5     (0x00000004)
#define SCB_PLLCFG_MUL6     (0x00000005)
#define SCB_PLLCFG_MUL7     (0x00000006)
#define SCB_PLLCFG_MUL8     (0x00000007)
#define SCB_PLLCFG_MUL9     (0x00000008)
#define SCB_PLLCFG_MUL10    (0x00000009)
#define SCB_PLLCFG_MUL11    (0x0000000a)
#define SCB_PLLCFG_MUL12    (0x0000000b)
#define SCB_PLLCFG_MUL13    (0x0000000c)
#define SCB_PLLCFG_MUL14    (0x0000000d)
#define SCB_PLLCFG_MUL15    (0x0000000e)
#define SCB_PLLCFG_MUL16    (0x0000000f)
#define SCB_PLLCFG_MUL17    (0x00000010)
#define SCB_PLLCFG_MUL18    (0x00000011)
#define SCB_PLLCFG_MUL19    (0x00000012)
#define SCB_PLLCFG_MUL20    (0x00000013)
#define SCB_PLLCFG_MUL21    (0x00000014)
#define SCB_PLLCFG_MUL22    (0x00000015)
#define SCB_PLLCFG_MUL23    (0x00000016)
#define SCB_PLLCFG_MUL24    (0x00000017)
#define SCB_PLLCFG_MUL25    (0x00000018)
#define SCB_PLLCFG_MUL26    (0x00000019)
#define SCB_PLLCFG_MUL27    (0x0000001a)
#define SCB_PLLCFG_MUL28    (0x0000001b)
#define SCB_PLLCFG_MUL29    (0x0000001c)
#define SCB_PLLCFG_MUL30    (0x0000001d)
#define SCB_PLLCFG_MUL31    (0x0000001e)
#define SCB_PLLCFG_MUL32    (0x0000001f)
#define SCB_PLLCFG_DIV1     (0x00000000)
#define SCB_PLLCFG_DIV2     (0x00000020)
#define SCB_PLLCFG_DIV4     (0x00000040)
#define SCB_PLLCFG_DIV8     (0x00000060)
#define SCB_PLLCFG_MASK     (0x0000007f)

#define SCB_PLLCON_PLLE     (0x00000001)
#define SCB_PLLCON          (*(pREG32 (0xe01fc080)))
#define SCB_PLLCON_PLLE     (0x00000001)
#define SCB_PLLCON_PLLC     (0x00000002)
#define SCB_PLLCON_MASK     (0x00000003)

#define SCB_PLLFEED         (*(pREG32 (0xe01fc08c)))
#define SCB_PLLFEED_FEED1   (0x000000aa)
#define SCB_PLLFEED_FEED2   (0x00000055)

#define SCB_PLLSTAT         (*(pREG32 (0xe01fc088)))
#define SCB_PLLSTAT_MSEL    (0x0000001f)
#define SCB_PLLSTAT_PSEL    (0x00000060)
#define SCB_PLLSTAT_PLLE    (0x00000100)
#define SCB_PLLSTAT_PLLC    (0x00000200)
#define SCB_PLLSTAT_PLOCK   (0x00000400)

static void _initPLL() {
	//
	//  Setup the PLL to multiply the 12Mhz XTAL input by 4, divide by 1
	//
	SCB_PLLCFG = (SCB_PLLCFG_MUL4 | SCB_PLLCFG_DIV1);

	//
	//  Activate the PLL by turning it on then feeding the correct sequence of bytes
	//
	SCB_PLLCON = SCB_PLLCON_PLLE;
	SCB_PLLFEED = SCB_PLLFEED_FEED1;
	SCB_PLLFEED = SCB_PLLFEED_FEED2;

	//
	//  Wait for the PLL to lock...
	//
	while (!(SCB_PLLSTAT & SCB_PLLSTAT_PLOCK))
		;

	//
	//  ...before connecting it using the feed sequence again
	//
	SCB_PLLCON = SCB_PLLCON_PLLC | SCB_PLLCON_PLLE;
	SCB_PLLFEED = SCB_PLLFEED_FEED1;
	SCB_PLLFEED = SCB_PLLFEED_FEED2;
}

void initPLL() {

	PLLCFG = 0x24; //set multiplier/divider for 60MHz
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	PLLCON = 0x01; //enable PLL
	PLLFEED = 0xAA;
	PLLFEED = 0x55;
	while (!(PLLSTAT & SCB_PLLSTAT_PLOCK))
		; //wait for PLL to lock to set frequency
	PLLCON = 0x3; //connect PLL as clock source
	PLLFEED = 0xAA;
	PLLFEED = 0x55;

	MAMCR = 0x02; //enable MAM
	MAMTIM = 0x04; //set number of clocks used for flash memory fetch
	VPBDIV = 0x01; //set peripheral clock (pclk) to system clock (cclk)
}

JNIEXPORT void JNICALL Java_thinj_blueboard_Blueboard_init(JNIEnv *env, jclass cl) {
	//initPLL();

	lcd_init();
	lcd_putstring(0, "Welcome");
	//test_rs232();

}

void JNICALL Java_thinj_blueboard_Blueboard_rs232OutString(JNIEnv *env, jclass this, jstring s) {
	stackable* val = (stackable*) s;
	array* a = (array*) (val->operand.jref);
	int i;
	for (i = 0; i < a->header.length; i++) {
		char c = a->data[i];
		rs232_putchar(c);
	}
}

void JNICALL Java_thinj_blueboard_Blueboard_outString(JNIEnv *env, jclass this, jstring s) {
	lcd_gotoxy(0, 0);
	//	lcd_putstring(1, "outString");
	stackable* val = (stackable*) s;
	array* a = (array*) (val->operand.jref);
	int i;
	for (i = 0; i < a->header.length; i++) {
		char c = a->data[i];
		lcd_putchar(c);
	}
}

void JNICALL Java_thinj_blueboard_Blueboard_outInt(JNIEnv *env, jobject clazz, jint p0) {
	char s[17];
	int digits;
	if (p0 < 10) {
		digits = 1;
	} else if (p0 < 100) {
		digits = 2;
	} else if (p0 < 1000) {
		digits = 3;
	} else if (p0 < 10000) {
		digits = 4;
	} else {
		digits = 5;
	}
	int i;
	for (i = digits - 1; i >= 0; i--) {
		s[i] = '0' + (p0 % 10);
		p0 /= 10;
	}
	s[digits] = '\0';
	lcd_putstring(1, s);
}

void delay_ms(jint delayInMillis)
{
    int milli;
    int loop;
    for(milli = 0;milli < delayInMillis;milli++){
        for(loop = 0;loop < 2650;loop++){
        }
    }

}

void JNICALL Java_thinj_blueboard_Blueboard_delay(JNIEnv *env, jclass clazz, jint delayInMillis) {
    delay_ms(delayInMillis);
}

void JNICALL Java_thinj_blueboard_Blueboard_usbLed(JNIEnv *env, jclass clazz, BOOL ledOn) {
	// set P0.31 to output:
	IO0DIR |= (1 << 31);
	if (ledOn) {
		// Turn off USB LED:
		IO0CLR |= (1 << 31);
	} else {
		// Turn on USB LED:
		IO0SET |= (1 << 31);
	}
}

//-----------------------------------------------------------------------------------------
// Buzzer handling
//-----------------------------------------------------------------------------------------
#define BUZZER (1<<25)

#define BUZZER_DIR IO1DIR
#define BUZZER_SET IO1SET
#define BUZZER_CLR IO1CLR

void buzz(BOOL on)
{
    BUZZER_DIR |= BUZZER;
    if (on) {
		BUZZER_CLR |= BUZZER;
	} else {
		BUZZER_SET |= BUZZER;
	}
}


void JNICALL Java_thinj_blueboard_Blueboard_buzzer(JNIEnv *env, jclass clazz, BOOL on) {
	buzz(on);
}

