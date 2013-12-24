#include <stdio.h> //sprintf
#include <stdlib.h> //malloc
#include "local.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"
#include <util/delay.h>
#include <avr/interrupt.h>

void sound(uint16_t duration[])
{
	volatile int i;
	while(*duration--)
	{
		for(i = *(duration+1) ; i > 0; i--);
		PORTD ^= (1 << PD3);
	}
}




void func_menu11(char c) //Zegar czasu rzeczywistego - RTC
{
	switch(c)
	{
		//edycja zegara RTC
		case 1:
		{
			if(keys!=0)
			{
				switch(keys)
				{
					case 1: //Escape
					local--;
					break;

					case 2://MINUS
					{
						switch(local-1)
						{
							case 0:
							{
								if(time[local-1]>0)
								time[local-1]--;
								else
								time[local-1]=23;
							}
							break;
							case 1:
							case 2:
							{
								if(time[local-1]>0)
								time[local-1]--;
								else
								time[local-1]=59;
							}
							break;
						}
					}
					break;

					case 4://PLUS
					time[local-1]++;
					break;

					case 8://Enter
					if(local<3)
					local++;
					else
					local=1;
					break;
				}
				
				//wykonanie sie TYLKO podczas przycisniecia, nie ma potrzeby odswiezania
				//co chwile
				if(local!=0)
				{
					sprintf(lcd_buff,"\001\x01\004\xff");
					switch(local-1)
					{
						case 0: //godziny
						{
							time[local-1] %= 24;
							if(time[local-1] < 12)
							sprintf(lcd_buff,"\001\x01\004\xff\001\x0c\004\377\001\x83\004\377Hour (AM):\001\xc7\004\377%.2u\001\xcf\004\377%c",time[local-1], 127);
							else
							sprintf(lcd_buff,"\001\x01\004\xff\001\x0c\004\377\001\x83\004\377Hour (PM):\001\xc7\004\377%.2u\001\xcf\004\377%c",time[local-1], 127);
							
							lcd_buff_full=1;
						}
						break;
						
						case 1: //minuty
						{
							time[local-1] %= 60;
							sprintf(lcd_buff,"\001\x01\004\xff\001\x0c\004\377\001\x84\004\377Minutes:\001\xc7\004\377%.2u\001\xcf\004\377%c",time[local-1], 127);
							lcd_buff_full=1;
						}
						break;
						
						case 2: //sekundy
						{
							time[local-1] %= 60;
							sprintf(lcd_buff,"\001\x01\004\xff\001\x0c\004\377\001\x84\004\377Seconds:\001\xc7\004\377%.2u\001\xcf\004\377%c",time[local-1], 127);
							lcd_buff_full=1;
						}
						break;
					}
					keys = 0;
				}
				else
				keys=255;
			}
		}
		break;
		
		//wyświetlanie zegara co 1 s
		case 2:
		{
			if(time[2] == 60)
			time[1]++;
			if(time[1]==60)
			time[0]++;
			
			time[2] %= 60;
			time[1] %= 60;
			time[0] %= 24;
			
			//brak migania przez nadpisywanie
			sprintf(lcd_buff,"\001\x80\004\xff\001\x0c\004\377\001\x81\004\377%s\001\xc3\004\377 %.2u:%.2u:%.2u\001\xcf\004\377%c",txt2, time[0], time[1], time[2], 126);
			lcd_buff_full=1;
		}
		break;
	}
}

void func_menu12(char c)
{
	local = 0;
}

void func_menu1311(char c)
{
	local = 0;
}

void func_menu1312(char c) //metronome
{
	static uint16_t config[2]={100,100};
	switch(c)
	{
		case 1:
		{
			
			
			if(keys!=0)
			{
				switch(keys)
				{
					case 1: //Escape
					local--;
					break;

					case 2://MINUS
					config[local-1]--;
					break;

					case 4://PLUS
					config[local-1]++;
					break;

					case 8://Enter
					if(local<2)
					local++;
					else
					local=1;
					break;
				}
				
				//wykonanie sie TYLKO podczas przycisniecia, nie ma potrzeby odswiezania
				//co chwile
				if(local!=0)
				{
					switch(local-1)
					{
						case 0:
						{
							sprintf(lcd_buff, "\001\x01\004\377Duration");
							lcd_buff_full=1;
						}
						break;
						case 1:
						{
							sprintf(lcd_buff, "\001\x01\004\377pulse");
							lcd_buff_full=1;
						}
						break;
					}
				}
			}
			break;
			
			//wyświetlanie zegara co 1 s
			case 2:
			{
				sound(config);
			}
			break;
		}
		
	}
	
}
void func_menu21(char c) //voltmeter
{
	switch(c)
	{
		static uint8_t adc_toggle_volt=0;
		case 1:
		{
			if(keys!=0)
			{
				switch(keys)
				{
					case 1: //escape
					local--;
					break;

					case 2:
					;
					break;

					case 4:
					;
					break;

					case 8: //enter local
					local++;
					break;
				}
				if(local)
				{
					adc_toggle_volt = (adc_toggle_volt + 1) % 2;
					if(adc_toggle_volt) //turn on ADC, prescaler 64 (125kHz), free runing mode, enable interrupts, start conv
					{
						//ADC - ref. voltage from Vcc, PA7 to voltometer (111), PA6 to thermometer (110)
						ADMUX |= (1<<REFS0) | (1<<MUX0) | (1<<MUX1) | (1<<MUX2);
						ADCSRA |= (1<<ADEN) | (1<<ADATE) | (1<<ADIE) | (1<<ADSC) | (1<<ADPS2) | (1<<ADPS1);
					}
					else
					{
						ADMUX = 0x00;
						ADCSRA = 0x00;
					}
					local = 0; //go back
				}
			}
			else
			keys=255;
		}
		break;
		
		case 2:
		{
			first_time[1]=0; //thermo + distance (1,2)
			first_time[2]=0;
			
			if(first_time[0]==0)
			{
				adc_toggle_volt=0;
				ADMUX = 0x00;
				ADCSRA = 0x00;
				first_time[0]++;
			}
			float temp;
			int integer, fractional;
			const char on[]="on", off[]="off";
			if(adc_toggle_volt==0)
			{
				integer = 0;
				fractional = 0;
				sprintf(lcd_buff,"\001\x80\004\377%s       \001\xc0\004\377%.2d.%.3d V (%s)  ",txt8,integer, fractional, off);
				lcd_buff_full=1;
			}
			else
			{
				temp = measurement/204.6f;
				integer = (int) temp; //konwersja z float na uint8_t
				temp = (temp - integer) * 1000.0f;
				fractional = (int) temp;
				sprintf(lcd_buff,"\001\x80\004\377%s       \001\xc0\004\377%.2d.%.3d V (%s)   ",txt8,integer, fractional, on);
				lcd_buff_full=1;
			}
			
		}
		break;
	}
}
void func_menu22(char c) //thermometer
{
	switch(c)
	{
		static uint8_t adc_toggle_thermo=0;
		case 1:
		{
			if(keys!=0)
			{
				
				switch(keys)
				{
					case 1: //escape
					local--;
					break;

					case 2:
					;
					break;

					case 4:
					;
					break;

					case 8: //enter local
					local++;
					break;
				}
				if(local)
				{
					adc_toggle_thermo = (adc_toggle_thermo + 1) % 2;
					if(adc_toggle_thermo) //turn on ADC, prescaler 64 (125kHz), free runing mode, enable interrupts, start conv
					{
						//ADC - ref. voltage from Vcc, PA7 to voltometer (111), PA6 to thermometer (110)
						ADMUX |= (1<<REFS0) | (1<<MUX1) | (1<<MUX2);
						ADCSRA |= (1<<ADEN) | (1<<ADATE) | (1<<ADIE) | (1<<ADSC) | (1<<ADPS2) | (1<<ADPS1);
					}
					else
					{
						ADMUX = 0x00;
						ADCSRA = 0x00;
					}
					local = 0; //go back
				}
			}
			else
			keys=255;
		}
		break;
		
		case 2:
		{
			first_time[0]=0; //voltmeter + distance (0,2)
			first_time[2]=0;
			
			if(first_time[1]==0)
			{
				adc_toggle_thermo=0;
				ADMUX = 0x00;
				ADCSRA = 0x00;
				first_time[1]++;
			}
			/* SciDavis said:
			Linear Regression fit of dataset: Table1_2, using function: A*x+B
			Y standard errors: Unknown
			From x = 82 to x = 214
			B (y-intercept) = -20,5 +/- 9,99294951168512e-16
			A (slope) = 0,25 +/- 6,52646234769871e-18 */
			float temp;
			int integer, fractional;
			const char on[]="on", off[]="off";
			if(adc_toggle_thermo==0)
			{
				integer=0;
				fractional=0;
				sprintf(lcd_buff,"\001\x80\004\377%s     \001\xc0\004\377%.2d.%.3d C (%s)  ",txt9, integer, fractional, off);
				lcd_buff_full=1;
			}
			else
			{
				
				temp = (0.25f*measurement)-20.5f;
				integer = (int) temp; //conversion from float to uint8_t
				temp = (temp - integer) * 100.0f;
				fractional = (int) temp;
				sprintf(lcd_buff,"\001\x80\004\377%s     \001\xc0\004\377%.2d.%.3d C (%s)   ",txt9, integer, fractional, on);
				lcd_buff_full=1;
			}
			
		}
		break;
	}
}

void func_menu23(char c) //distance
{
	switch(c)
	{
		static uint8_t toggle_distance=0;
		case 1:
		{
			if(keys!=0)
			{
				
				switch(keys)
				{
					case 1: //escape
					local--;
					break;

					case 2:
					;
					break;

					case 4:
					;
					break;

					case 8: //enter local
					local++;
					break;
				}
				if(local)
				{
					toggle_distance = (toggle_distance + 1) % 2;
					if(toggle_distance)
					{
						PORTA |= (1<<PA1);
						_delay_us(15);
						PORTA &= ~(1<<PA1);
					}
					else
					{
						distance = 0;
					}
					local = 0; //go back
				}
			}
			else
			keys=255;
		}
		break;
		
		case 2:
		{
			first_time[0]=0; //voltmeter + thermo (0,1)
			first_time[1]=0;
			if(first_time[2]==0)
			{
				distance = 0;
				toggle_distance=0;
				first_time[2]++;
			}
			float temp;
			int integer, fractional;
			const char on[]="on", off[]="off";
			if(toggle_distance==0)
			{
				integer=0;
				fractional=0;
				sprintf(lcd_buff,"\001\x80\004\377%s (cm)\001\xc0\004\377%.3d.%.3d (%s)   ",txt12, integer, fractional, off);
				lcd_buff_full=1;
			}
			else
			{
				//temp = distance*3400.0f/2000.0f - from web, do not work correctly;
				/* SciDavis said:
				Linear Regression fit of dataset: Table1_2, using function: A*x+B
				Y standard errors: Unknown
				From x = 18 to x = 61
				B (y-intercept) = 0,472070075080443 +/- 0,432052891875861
				A (slope) = 1,6910207365034 +/- 0,0112965138865972 */
				temp = distance*1.69f+0.47f;
				integer = (int) temp; //conversion from float to int
				temp = (temp - integer) * 100.0f;
				fractional = (int) temp;
				sprintf(lcd_buff,"\001\x80\004\377%s (cm)\001\xc0\004\377%.3d.%.2d (%s)     ",txt12, integer, fractional, on);
				lcd_buff_full=1;
			}
			
		}
		break;
	}
}

void func_menu31(char c)
{
	local = 0;
}