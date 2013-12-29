#ifndef LCD_H
#define LCD_H

#include <avr/pgmspace.h> //progmem
#include <stdlib.h> //malloc
#include <stdio.h> //sprintf

extern uint8_t volatile lcd_buff_full;
extern char *lcd_buff;

struct lcd_field
{
	volatile char rs : 1;
	volatile char rw : 1;
	volatile char e : 1;
	volatile char : 1; //I don't use 4th bit
	volatile uint8_t data : 4; //data bits (lcd)
};

#define lcd_rs ((*((struct lcd_field *) &PORTB)).rs) 
#define lcd_rs_dir (((struct lcd_field *) &DDRB) -> rs)

#define lcd_rw ((*((struct lcd_field *) &PORTB)).rw)
#define lcd_rw_dir (((struct lcd_field *) &DDRB) -> rw)

#define lcd_e ((*((struct lcd_field *) &PORTB)).e)
#define lcd_e_dir (((struct lcd_field *) &DDRB) -> e)

#define lcd_data ((*((struct lcd_field *) &PORTD)).data)
#define lcd_data_dir (((struct lcd_field *) &DDRD) -> data)

void lcd_initiation (char data, char rs);
void lcd_write_data(char data);
void lcd_write_command(char data);
void lcd_show(void);

#endif