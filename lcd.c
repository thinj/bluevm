#include "lpc_2148.h"
#include "lcd.h"
#include "util.h"

#define LCD_BACK_LIGHT_TIMEOUT 1000

#define LCD_BACKLIGHT       BIT21

#define LCD_BACK_LIGHT_DIR 	IO1DIR
#define LCD_BACK_LIGHT_SET	IO1SET
#define LCD_BACK_LIGHT_CLR 	IO1CLR

#define LCD_DATA_DIR	   	IO0DIR
#define LCD_DATA_SET	   	IO0SET
#define LCD_DATA_CLR	   	IO0CLR

#define LCD_CTRL_DIR		IO1DIR
#define LCD_CTRL_SET        IO1SET
#define LCD_CTRL_CLR        IO1CLR

#define LCDRS	            (1<<24)
#define LCDRW	            (1<<23)
#define LCDEN	            (1<<22)
#define BIT21               (1<<21)

#define LCD_D4              (1<<10)
#define LCD_D5              (1<<11)
#define LCD_D6              (1<<12)
#define LCD_D7              (1<<13)

#define LCD_DATA_MASK       (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)
#define LCD_BUSY_FLAG       LCD_D7

#define LCD_CONTROL_MASK    0x01C00000

typedef unsigned char U8;
typedef unsigned int U32;


static void usb_on() {
	// set P0.31 to output:
	IO0DIR |= (1 << 31);

	// Turn on USB LED:
	IO0CLR |= (1 << 31);
}

void writeControl4BitsNoDelay(U8 data) {
	U8 temp = 0;
	U32 temp1 = 0;

	temp = data & 0x0F;
	temp1 = (temp << 10) & LCD_DATA_MASK;

	LCD_CTRL_CLR = LCDRS;
	LCD_CTRL_SET = LCDEN;
	delay(2);
	LCD_DATA_CLR = LCD_DATA_MASK;
	LCD_DATA_SET = temp1;
	delay(2);
	LCD_CTRL_CLR = LCDEN;
	delay(2);
}

void lcd_delay() {
	//delay(100000);
	delay(30);
}

void writeControl(U8 x) {
	writeControl4BitsNoDelay(x >> 4);
	writeControl4BitsNoDelay(x);

	lcd_delay();

}

void writeData4BitsNoDelay(U8 data) {
	U8 temp = 0;
	U32 temp1 = 0;

	temp = data & 0x0F;
	temp1 = (temp << 10) & LCD_DATA_MASK;

	LCD_CTRL_SET = LCDEN | LCDRS;
	LCD_DATA_CLR = LCD_DATA_MASK;
	LCD_DATA_SET = temp1;
	LCD_CTRL_CLR = LCDEN;
}

void writeData(U8 x) {
	writeData4BitsNoDelay(x >> 4);
	writeData4BitsNoDelay(x);

//	lcd_delay();
	delay(20L);
}

/////////////////////////

void lcd_clear(void) {
	writeControl(0x01);
}
void lcd_gotoxy(unsigned int x, unsigned int y) {
	if (y == 0) {
		writeControl(0x80 + x);
	} else if (y == 1) {
		writeControl(0xC0 + x);
	}
}

static void init_port(void) {
	LCD_CTRL_DIR |= (LCDEN | LCDRS | LCDRW);
	LCD_CTRL_CLR |= (LCDEN | LCDRS | LCDRW);
	LCD_DATA_DIR |= LCD_DATA_MASK;
}

void lcd_init(void) {
	usb_on();

	init_port();

	delay(30*1000);

	// Ninja magic begin:
	writeControl4BitsNoDelay(3);
	delay(10*1000);
	writeControl4BitsNoDelay(3);
	delay(1*1000);
	writeControl4BitsNoDelay(3);
	writeControl4BitsNoDelay(2);
//	usb_on();

	writeControl(0x2c); // 4 bit, dual line, 5x10
	writeControl(0x0f); // display on, cursor on, cursor blink on
	writeControl(0x06); // increment; display shift off
	writeControl(0x1c); // move cursor; shift rigth
	writeControl(0x01); // clear display

	delay(1000);

	//writeControl(0xc0);
//	lcd_gotoxy(5, 1);
//	writeData('H');
//	writeData('o');
//	writeData('w');
//	writeData('d');
//	writeData('y');
//	writeData('@');
//	usb_blink();
}

// Backlight:
//	IO1DIR |= (1 << 21);
//	while (1) {
//		// Turn off backlight:
//		IO1CLR |= (1 << 21);
//		delay(500);
//		// Turn on backlight:
//		IO1SET |= (1 << 21);
//		delay(500);
//	}

void lcd_putchar(int c) {
	writeData(c);
}

void lcd_putstring(unsigned char line, char *cp) {
   lcd_gotoxy(0, line);
   while (*cp) {
	   writeData(*cp);
	   cp++;
   }
}
