/*
Debouncing przyciskow (4 najstarsze bity portu C)
Menu oparte na strukturach
Poczatek implementacji funkcji lokalnych do glownego menu
*/

#include <avr/io.h>
#include <avr/pgmspace.h> //PROGMEM
#include <stdlib.h> //malloc
#include <stdio.h> //sprintf
#include <avr/interrupt.h>
#include <stdbool.h>
#include "local.h"
#include "lcd.h"
#include "buttons.h"
#include "i2c.h"

/* global variables (interrupts etc.) */
uint8_t volatile keys, local, lcd_buff_full;
char *lcd_buff; //pointer to buffer
volatile _Bool run_function=false, lcd_start=false; //can we run function(2)?
uint8_t first_time[3]={0};
unsigned int volatile measurement = 0, distance = 0; //adc + HC-SR04 distance sensor

const uint8_t PROGMEM moves[8]={1, 3, 2, 6, 4, 12, 8, 9}; //stepper motor

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

struct menu M1, M2, M3, M11, M21, M22, M23, M31;
struct menu *current_menu;

//menu 1 - real time clock, stopwatch, count down
const char txt1[] = "Clock/Timers    ";
const char txt2[] = "RTC";
//menu 2 - voltmeter, thermometer
const char txt3[] = "Voltage/Temp    \001\xc0\004\377Distance";
const char txt4[] = "Voltmeter";
const char txt5[] = "Thermometer";
//menu 3 - stepper motor
const char txt6[] = "Stepper Motor   ";
const char txt7[] = "SM Config";
const char txt8[] = "Distance";

struct menu M1 = {&M3, &M2, NULL, &M11, txt1, NULL, 0, 126};
struct menu M11 = {NULL, NULL, &M1, NULL, txt2, &func_menu11, 7500, 126};

struct menu M2 = {&M1, &M3, NULL, &M21, txt3, NULL, 0, 126};
struct menu M21 = {&M23, &M22, &M2, NULL, txt4, &func_menu21, 1000, 127};
struct menu M22 = {&M21, &M23, &M2, NULL, txt5, &func_menu22, 1000, 127};
struct menu M23 = {&M22, &M21, &M2, NULL, txt8, &func_menu23, 1000, 127};	

struct menu M3 = {&M2, &M1, NULL, &M31, txt6, NULL, 0, 126};
struct menu M31 = {NULL, NULL, &M3, NULL, txt7, &func_menu31, 10, 127};

// -- wait \004\377 -- command \001\x28 --
const char lcd_init[] PROGMEM = "\004\377\001\x28\004\377\001\x28\004\377\001\x28\004\377\001\x0c\004\377\001\x06\004\377\001\x01\004\377";

int main (void)
{
	//Timer0, CTC mode, 10kHz
	TCCR0 |= _BV(CS01) | (1 << WGM01);
	TIMSK |= (1 << OCIE0);
	OCR0 = 99;
	
	//buzzer configuration
	buzzer_dir = 1;
	buzzer = 0;
	
	//distance sensor conf
	DDRA = (1<<PA1);
	PORTA &= ~(1<<PA0) | ~(1<<PA1);
	
	//deklaracja przyciskow i polaryzacja
	hardware_keys_port = 0xf; //f lub 15 - bo tylko robie pull up na 4 bitach!

	//inicjalizacja LCD
	lcd_rs_dir = 1;
	lcd_rw_dir = 1;
	lcd_e_dir = 1;
	lcd_data_dir = 0xf;
	
	//stepper conf
	DDRD |= 0xf;
	
	I2C_initiation();	
	time.seconds = I2C_get_value(REG_SECONDS, NACK);
	time.minutes = I2C_get_value(REG_MINUTES, NACK);
	time.hours = I2C_get_value(REG_HOURS, NACK);
	date.day_week = I2C_get_value(REG_DAY_OF_THE_WEEK, NACK);
	date.day_month = I2C_get_value(REG_DAY_OF_THE_MONTH, NACK);
	date.month = I2C_get_value(REG_MONTH, NACK);
	date.year = I2C_get_value(REG_YEAR, ACK);
		
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
				sprintf(lcd_buff,"\001\x01\004\377%s",current_menu->str);
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
		}
	}
	return 0;
}

ISR(TIMER0_COMP_vect) //10 k Hz = 100 us
{
	uint16_t static period_cnt, period_clock=5000;
	
	if(PINA & _BV(PA0)) //distance sensor
	distance++;

	if(period_clock == 0) //lcd start after 1s
	{
		lcd_start=true;
	}
	else if(period_clock> 0)
	period_clock--;

	buttons_debouncing();
	lcd_show();

	if(current_menu -> period != 0) //do function(2) with specified period (time)
	{
		if(period_cnt == 0)
		{
			period_cnt = current_menu -> period;
			
			if((local == 0) && ((current_menu -> function) != NULL))
			{
				//(*((*current_menu).function))(2); - too long to handle in ISR!
				run_function = true; //so, do it in main()
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
