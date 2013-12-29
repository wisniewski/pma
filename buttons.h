﻿#ifndef BUTTONS_H
#define BUTTONS_H

#include <avr/io.h>

extern uint8_t volatile keys; //buttons

struct button_field
{
	volatile uint8_t : 4;
	volatile uint8_t k: 4;
};

#define hardware_keys (((struct button_field *) &PINC) -> k)
#define hardware_keys_dir (((struct button_field *) &DDRC) -> k)
#define hardware_keys_port (((struct button_field *) &PORTC) -> k)

void buttons_debouncing(void);

#endif