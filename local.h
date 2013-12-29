#ifndef LOCAL_H
#define LOCAL_H

#include <stdio.h> //sprintf
#include <stdlib.h> //malloc
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"
#include <avr/pgmspace.h>
#include "i2c.h"
#include <string.h>

typedef struct rtc_time
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
} rtc;
rtc time;

typedef struct rtc_date
{
	uint8_t day_week;
	uint8_t day_month;
	uint8_t month;
	uint8_t year;
} rtc2;
rtc2 date;

extern uint8_t volatile keys; //ktory przycisk nacisniety?
extern uint8_t volatile local;
extern uint8_t volatile lcd_buff_full;
extern char *lcd_buff;
extern volatile _Bool run_function;
extern unsigned int volatile measurement;
extern uint8_t first_time[3];
extern unsigned int volatile distance;

extern const uint8_t moves[8];

extern const char txt1[];
extern const char txt2[];
extern const char txt3[];
extern const char txt4[];
extern const char txt5[];
extern const char txt6[];
extern const char txt7[];
extern const char txt8[];
extern const char txt9[];
extern const char txt10[];
extern const char txt11[];
extern const char txt12[];

//deklaracje funkcji lokalnych
void func_menu11(char c);
void func_menu21(char c);
void func_menu22(char c);
void func_menu23(char c);
void func_menu31(char c);

#endif