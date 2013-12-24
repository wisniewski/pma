/*
Debouncing przyciskow (4 najstarsze bity portu C)
Menu oparte na strukturach
Poczatek implementacji funkcji lokalnych do glownego menu
*/

#include <avr/io.h> //I/O, rejestry itd.
#include <avr/pgmspace.h> //progmem
#include <stdlib.h> //malloc
#include <stdio.h> //sprintf
#include <avr/interrupt.h> //przerwania
#include <stdint.h>
#include <stdbool.h>
#include "local.h"
#include "lcd.h"
#include "buttons.h"

/* global variables */
uint8_t volatile keys; //buttons
uint8_t volatile local; //local functions
uint8_t volatile lcd_buff_full; //write from buffer to lcd
char *lcd_buff; //pointer to buffer
volatile _Bool run_function=false, lcd_start=false; //can we run function(2)?
unsigned int time[3]={0,0,0}; //rtc
unsigned int measurement = 0;

//struktura menu // <-------- --------- ------- M E N U
struct menu
{
	struct menu *left;
	struct menu *right;
	struct menu *up;
	struct menu *down;
	const char *str; //string
	void (*function)(char); //pointer to function
	uint16_t period; //how fast handle function(2)
	char arrow;
};

struct menu M1, M2, M3, M11, M12, M13, M131, M132, M21, M22, M31;
struct menu *current_menu;

//menu 1 - real time clock, stopwatch, count down
const char txt1[] = "\001\x82\004\377Clock/Timers\001\xc4\004\377Counters";
const char txt2[] = "\001\x81\004\377Real Time Clock";
const char txt3[] = "\001\x83\004\377Stop Watch";
const char txt4[] = "\001\x83\004\377Count Down";
const char txt5[] = "\001\x84\004\377Egg Timer";
const char txt6[] = "\001\x84\004\377Metronome";
//menu 2 - voltmeter, thermometer
const char txt7[] = "\001\x83\004\377Voltmeter\001\xc2\004\377Thermometer";
const char txt8[] = "\001\x83\004\377Voltmeter";
const char txt9[] = "\001\x82\004\377Thermometer";
//menu 3 - motor
const char txt10[] = "\001\x85\004\377Motor\001\xc4\004\377Control";
const char txt11[] = "PodMenu 3-1";

//definicja odwoluje sie do innych elementow {lewo, prawo, gora nic, dol, "string"}
struct menu M1 = {&M3, &M2, NULL, &M11, txt1, NULL, 0, 126};
struct menu M11 = {&M13, &M12, &M1, NULL, txt2, &func_menu11, 100, 126};
struct menu M12 = {&M11, &M13, &M1, NULL, txt3, &func_menu12, 0, 127};
struct menu M13 = {&M12, &M11, &M1, &M131, txt4, NULL, 0, 127};
struct menu M131 = {&M132, &M132, &M13, NULL, txt5, &func_menu1311, 0, 127};
struct menu M132 = {&M131, &M131, &M13, NULL, txt6, &func_menu1312, 1000, 127};

struct menu M2 = {&M1, &M3, NULL, &M21, txt7, NULL, 0, 126};
struct menu M21 = {&M22, &M22, &M2, NULL, txt8, &func_menu21, 1000, 127};
struct menu M22 = {&M21, &M21, &M2, NULL, txt9, &func_menu22, 1000, 127};

struct menu M3 = {&M2, &M1, NULL, &M31, txt10, NULL, 0, 126};
struct menu M31 = {NULL, NULL, &M3, NULL, txt11, &func_menu31, 0, 127};

//wait \004\377
//command \001\x28
const char lcd_init[] PROGMEM = "\004\377\001\x28\004\377\001\x28\004\377\001\x28\004\377\001\x0c\004\377\001\x06\004\377\001\x01\004\377";

ISR(TIMER0_COMP_vect) //10 k Hz = 100 us <-------- P R Z E R W A N I E
{
	uint16_t static period_cnt, period_clock=10000;
	
	if(period_clock == 0)
	{
		time[2]++;
		lcd_start=true;
		period_clock=10000;
		
	}
	else if(period_clock> 0)
	period_clock--;

	buttons_debouncing(); //obsluga przyciskow w przerwaniu
	lcd_show(); //obsluga LCD w przerwaniu

	if(current_menu -> period != 0)
	{
		if(period_cnt == 0)
		{
			//przypisanie do period_cnt wartosci z danego menu
			period_cnt = current_menu -> period;

			if((local == 0) && ((current_menu -> function) != NULL))
			{
				//(*((*current_menu).function))(2); - czas wykonania dluzszy niz ISR
				run_function = true;
			}
		}
		else if(period_cnt > 0)
		period_cnt--;
	}
}

ISR(ADC_vect)
{
	measurement = ADC;
}

int main (void) // <-------- -------- -------- -------- ----- M A I N
{
	//Timer0, CTC mode, 10kHz
	TCCR0 |= _BV(CS01) | (1 << WGM01);
	TIMSK |= (1 << OCIE0);
	OCR0 = 99;
	
	//buzzer configuration
	buzzer_dir = 1;
	buzzer = 0;
	
	//deklaracja przyciskow i polaryzacja
	hardware_keys_port = 0xf; //f lub 15 - bo tylko robie pull up na 4 bitach!

	//inicjalizacja LCD
	lcd_rs_dir = 1;
	lcd_rw_dir = 1;
	lcd_e_dir = 1;
	lcd_data_dir = 0xf;
	
	//wyswietlanie napisu na lcd
	lcd_buff = malloc(80);
	sprintf_P(lcd_buff, lcd_init);
	lcd_buff_full = 1;
	
	//na poczatek wyswietlajmy napis z menu 1
	keys = 16;
	current_menu = &M1;
	sei();
	while(lcd_start==false); //wait 1 second
	
	//obsluga przyciskow i poruszanie sie po menu
	while(1)
	{
		if(local == 0)
		{
			if(keys != 0)
			{
				switch(keys)
				{
					case 1:
					if((current_menu -> up) != NULL)
					{
						current_menu = current_menu -> up;
						sprintf(lcd_buff, "\001\x01\004\xff%s",current_menu->str); //czysci
						lcd_buff_full = 1;
					}
					break;

					case 2:
					if((current_menu -> left) != NULL)
					{
						current_menu = current_menu -> left;
						sprintf(lcd_buff, "\001\x01\004\xff%s",current_menu->str); //czysci
						lcd_buff_full = 1;
					}
					break;

					case 4:
					if((current_menu -> right) != NULL)
					{
						current_menu = current_menu -> right;
						sprintf(lcd_buff, "\001\x01\004\xff%s",current_menu->str); //czysci
						lcd_buff_full = 1;
					}
					break;

					case 8:
					if((current_menu -> down) != NULL)
					{
						current_menu = current_menu -> down;
						sprintf(lcd_buff, "\001\x01\004\xff%s",current_menu->str); //czysci
						lcd_buff_full = 1;
					}
					else if((current_menu -> function) != NULL)
					local = 1;
					break;
				}
				//jezeli nic nie naciskalismy to niech sie wyswietla obecne menu
				sprintf(lcd_buff,"\001\x01\004\377%s\001\xcf\004\377%c",current_menu->str, current_menu->arrow);
				lcd_buff_full = 1;
				keys = 0;
			}
			if(run_function)
			{
				(*((*current_menu).function))(2);
				run_function = false;
			}
		}
		else
		{
			(*((*current_menu).function))(1);
			keys = 255;
		}
		
		
	}
	return 0;
}
