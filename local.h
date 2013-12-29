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

extern uint8_t volatile keys;
extern uint8_t volatile local;
extern uint8_t volatile lcd_buff_full;
extern char *lcd_buff;
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

//local functions
void real_time_clock(char c);
void voltmeter(char c);
void thermometer(char c);
void distance_sensor(char c);
void stepper_motor(char c);

#endif