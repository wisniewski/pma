// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//							DOCUMENTATION
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// AUTHOR: Mariusz Wiœniewski // www.mariuszbonifacy.pl
// E-mail: mariusz.bonifacy@gmail.com
// DATE: December 2013
// TYPE: End of course project (AVR Programming in C)
// DEVELOPMENT BOARD: ZL15AVR with ATmega32
// SOFTWARE: Atmel Studio 6.1
// Atmel AVR (8 bit) GNU Toolchain: 3.4.2.939
// GCC_VERSION: 4.7.2
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//				SETTINGS AND CONNECTIONS ON ZL15AVR
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
// LCD character display (HD44780 compatible) in 4-bit mode:
// RS -> PB0
// RW -> PB1
// E -> PB2
// D4 - D7 -> PD4 - PD7
// 
// Buttons (with pull-up resistor):
// SW0 - SW3 -> PC4 - PC7
// 
// I2C (TWI) serial bus - RTC M41T00 with 32.768 kHz crystal:
// SCL -> PC0
// SDA -> PC1
// 
// Voltmeter:
// P2 -> PA7
// 
// Temperature (thermistor) MCP 9701:
// TEM -> PA6
// 
// Distance sensor HC-SR04:
// Trig -> PA1
// Echo -> PA0
// 
// Stepper Motor 28BYJ-48 with ULN2003APG driver:
// 1N1 - 1N4 -> BC547B npn transistor -> PD0 - PD3
// Emitter to 1Nx, Base with 220R resistor to PDx, Collector to Vcc
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//							OTHER MATTERS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// My cat and white cat: wwww.kocurkolandia.mariuszbonifacy.pl
// All calculations are based on official documentations (Microchip).
// I used SciDavis to linear regression fit.
// 
// I2C serial bus: with JP1 and JP2 jumpers (pull-up resistors).
// Microcontroller: ATmega32
// Frequency: 8 MHz
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include <avr/io.h>
#include <avr/pgmspace.h> //PROGMEM
#include <stdlib.h> //malloc
#include <stdio.h> //sprintf
#include <avr/interrupt.h>
#include "local.h"
#include "lcd.h"
#include "buttons.h"
#include "i2c.h"

/* global variables (interrupts etc.) */
uint8_t volatile keys, local, lcd_buff_full;
char *lcd_buff; //pointer to buffer
volatile char run_function=0, lcd_start=0; //can we run function(2)?
uint8_t first_time[3]={0};
unsigned int volatile measurement = 0, distance = 0; //adc + HC-SR04 distance sensor

const uint8_t PROGMEM moves[8]={1, 3, 2, 6, 4, 12, 8, 9}; //stepper motor
// -- wait \004\377 -- command \001\x28 --
const char lcd_init[] PROGMEM = "\004\377\001\x28\004\377\001\x28\004\377\001\x28\004\377\001\x0c\004\377\001\x06\004\377\001\x01\004\377";

struct menu
{
	struct menu *left;
	struct menu *right;
	struct menu *up;
	struct menu *down;
	const char* str; //string
	void (*function)(char); //pointer to function
	uint16_t period; //how fast handle function(2)
};

struct menu M1, M2, M3, M11, M21, M22, M23, M31;
struct menu *current_menu;

//menu 1 - real time clock, stopwatch, count down
const char txt1[] PROGMEM = "  RTC and Date  ";
const char txt2[] PROGMEM = "RTC";
//menu 2 - voltmeter, thermometer
const char txt3[] PROGMEM = "  Voltage/Temp  \001\xc0\004\377    Distance    ";
const char txt4[] PROGMEM = "Voltmeter";
const char txt5[] PROGMEM = "Thermometer";
//menu 3 - stepper motor
const char txt6[] PROGMEM = "    Stepper    \001\xc0\004\377     Motor     ";
const char txt7[] PROGMEM = "SM Config";
const char txt8[] PROGMEM = "Distance";

const char txt_menu[] PROGMEM = "\001\x01\004\xff%S";

struct menu M1 = {&M3, &M2, NULL, &M11, txt1, NULL, 0};
struct menu M11 = {NULL, NULL, &M1, NULL, txt2, &real_time_clock, 7500};
struct menu M2 = {&M1, &M3, NULL, &M21, txt3, NULL, 0};
struct menu M21 = {&M23, &M22, &M2, NULL, txt4, &voltmeter, 1000};
struct menu M22 = {&M21, &M23, &M2, NULL, txt5, &thermometer, 1000};
struct menu M23 = {&M22, &M21, &M2, NULL, txt8, &distance_sensor, 1000};	
struct menu M3 = {&M2, &M1, NULL, &M31, txt6, NULL, 0};
struct menu M31 = {NULL, NULL, &M3, NULL, txt7, &stepper_motor, 10};

int main (void)
{
	TCCR0 |= _BV(CS01) | (1 << WGM01); //Timer0, CTC mode, 10kHz
	TIMSK |= (1 << OCIE0);
	OCR0 = 99;
	
	DDRA = (1<<PA1); //distance sensor conf
	PORTA &= ~(1<<PA0) | ~(1<<PA1);
	
	hardware_keys_port = 0xf; //buttons init
	
	lcd_rs_dir = 1; //LCD conf
	lcd_rw_dir = 1;
	lcd_e_dir = 1;
	lcd_data_dir = 0xf;
	
	DDRD |= 0xf; //stepper conf
	
	I2C_initiation(); //TWI bus init and get values of time/date
	I2C_get_time_and_date();
		
	lcd_buff = malloc(80); //initialize LCD
	sprintf_P(lcd_buff, lcd_init);
	lcd_buff_full = 1;
	
	keys = 255;
	current_menu = &M1; //show first menu
	sei();
	while(lcd_start==0); //wait 500 ms

	while(1) //read buttons and navigate through the menu
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
						current_menu = current_menu -> up; //show menu and move on
						sprintf_P(lcd_buff, txt_menu,current_menu->str);
						lcd_buff_full = 1;
					}
					break;

					case 2:
					if((current_menu -> left) != NULL)
					{
						current_menu = current_menu -> left;
						sprintf_P(lcd_buff, txt_menu,current_menu->str);
						lcd_buff_full = 1;
					}
					break;

					case 4:
					if((current_menu -> right) != NULL)
					{
						current_menu = current_menu -> right;
						sprintf_P(lcd_buff, txt_menu,current_menu->str);
						lcd_buff_full = 1;
					}
					break;

					case 8:
					if((current_menu -> down) != NULL)
					{
						current_menu = current_menu -> down;
						sprintf_P(lcd_buff, txt_menu,current_menu->str);
						lcd_buff_full = 1;
					}
					else if((current_menu -> function) != NULL)
					local = 1;
					break;
				}
				sprintf_P(lcd_buff,txt_menu,current_menu->str); //let's show our current menu
				lcd_buff_full = 1;
				keys = 0;
			}
			if(run_function)
			{
				(*((*current_menu).function))(2);
				run_function = 0;
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
	
	if(PINA & _BV(PA0)) //distance sensor - read when it send high state
	distance++;

	if(period_clock == 0) //lcd start after 500 ms
	{
		lcd_start=1;
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
				run_function = 1; //so, do it in main()
			}
		}
		else if(period_cnt > 0)
		period_cnt--;
	}
}

ISR(ADC_vect) //interrupt when measurement is ready
{
	measurement = ADC;
}
