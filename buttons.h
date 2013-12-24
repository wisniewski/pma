#ifndef BUTTONS_H
#define BUTTONS_H

extern uint8_t volatile keys; //ktory przycisk nacisniety?

//pole bitowe do przechowywania stanow przyciskow
struct button_field
{
	volatile char k: 4;
};

//definiowane nazw i dostep do poszczegolnych bitow w polu <-------- D E F I N I C J E
#define hardware_keys (((struct button_field *) &PINB) -> k)
#define hardware_keys_dir (((struct button_field *) &DDRB) -> k)
#define hardware_keys_port (((struct button_field *) &PORTB) -> k)

//deklaracje funkcji
void buttons_debouncing(void);

#endif