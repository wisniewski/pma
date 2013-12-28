#include "local.h"

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
	
	switch(c)
	{
		case 1:
		{
			/*if(keys!=0)
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
			}*/
		}
		break;
		
		//wyświetlanie zegara co 1 s
		case 2:
		{
			time.seconds = I2C_get_value(REG_SECONDS, NACK);
			time.minutes = I2C_get_value(REG_MINUTES, NACK);
			time.hours = I2C_get_value(REG_HOURS, NACK);
			
			date.day_week = I2C_get_value(REG_DAY_OF_THE_WEEK, NACK);
			date.day_month = I2C_get_value(REG_DAY_OF_THE_MONTH, NACK);
			date.month = I2C_get_value(REG_MONTH, NACK);
			date.year = I2C_get_value(REG_YEAR, ACK);
			
			char day_name[4];
			switch(date.day_week)
			{
				case 1: {strcpy(day_name,"Mon");} break;
				case 2: {strcpy(day_name,"Tue");} break;
				case 3: {strcpy(day_name,"Wed");} break;
				case 4: {strcpy(day_name,"Thu");} break;
				case 5: {strcpy(day_name,"Fri");} break;
				case 6: {strcpy(day_name,"Sat");} break;
				case 7: {strcpy(day_name,"Sun");} break;
			}
			sprintf(lcd_buff,"\001\x80\004\xff%s: %.2d:%.2d:%.2d\001\xc0\004\xff%s %.2d.%.2d.%.4d",txt2, time.hours, time.minutes, time.seconds, day_name, date.day_month, date.month, date.year+2000);
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

					case 8: //enter local
					local++;
					break;
				}
				if(local)
				{
					toggle_distance = (toggle_distance + 1) % 2;
					if(toggle_distance==0)
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
				
				PORTA |= (1<<PA1); //send high state
				for(uint8_t i = 100; i>0; i--) //80 -> 10 us, ~100 us for certainty
				asm volatile ("nop");
				PORTA &= ~(1<<PA1); //and afer ~100 us
				
				temp = distance*1.69f+0.47f; //show on lcd distance
				integer = (int) temp; //conversion from float to int
				temp = (temp - integer) * 100.0f;
				fractional = (int) temp;
				sprintf(lcd_buff,"\001\x80\004\377%s (cm)\001\xc0\004\377%.3d.%.2d (%s)     ",txt12, integer, fractional, on);
				lcd_buff_full=1;
				
				distance=0; //for new measurement
			}
		}
		break;
	}
}
void func_menu31(char c) //Stepper motor
{
	typedef struct motor_data
	{
		unsigned int slowdown;
		unsigned int direction;
		unsigned int steps;
		uint8_t temporary_slowdown;
		uint16_t temporary_steps;
	} stepper;
	
	static stepper motor={0,0,0,0,0};

	
	switch(c)
	{
		case 1:
		{
			if(keys!=0)
			{
				switch(keys)
				{
					case 1: //Escape
					{
						local--;
						keys=0;
					}
					break;

					case 2://MINUS
					{
						switch(local-1)
						{
							case 0: //slowdown 0 - 10
							{
								motor.slowdown = (motor.slowdown+10)%11;
								motor.temporary_slowdown = motor.slowdown;
							}
							break;
							
							case 1: //direction 0 - 1 (left - right)
							{
								motor.direction = (motor.direction+1)%2;
							}
							break;
							
							case 2: //direction 0 - 1 (left - right)
							{
								motor.steps = (motor.steps+4160)%4224;
								
							}
							break;
						}
						keys=0;
					}
					break;

					case 4://PLUS
					{
						switch(local-1)
						{
							case 0: //slowdown 0 - 10
							{
								motor.slowdown = (motor.slowdown+1)%11;
								motor.temporary_slowdown = motor.slowdown;
							}
							break;
							
							case 1: //direction 0 - 1 (left - right)
							{
								motor.direction = (motor.direction+1)%2;
							}
							break;
							
							case 2: //direction 0 - 1 (left - right)
							{
								motor.steps = (motor.steps+64)%4224;
							}
							break;
						}
						keys=0;
					}
					break;

					case 8://Enter
					{
						local = (local+1)%4;
						keys=0;
					}
					break;
				}
			}
			
			//wykonanie sie TYLKO podczas przycisniecia, nie ma potrzeby odswiezania co chwile
			if(local)
			{
				switch(local-1)
				{
					case 0: //slowdown
					{
						if(motor.slowdown==0)
						sprintf(lcd_buff,"\001\x80\004\377Slowdown: none  ");
						else
						sprintf(lcd_buff,"\001\x80\004\377Slowdown: %ux    ",motor.slowdown);
						lcd_buff_full=1;
					}
					break;
					
					case 1: //direction
					{
						if(motor.direction)
						sprintf(lcd_buff,"\001\x80\004\377Direction: CW   ");
						else
						sprintf(lcd_buff,"\001\x80\004\377Direction: CCW  ");
						lcd_buff_full=1;
					}
					break;
					
					case 2: //steps
					{
						/* SciDavis said:
						Linear Regression fit of dataset: Table1_2, using function: A*x+B
						Y standard errors: Unknown
						From x = 5,625 to x = 360
						B (y-intercept) = 0 +/- 7,56647835265711e-18
						A (slope) = 11,3777777777778 +/- 3,5982833466408e-20 */
						
						float angle;
						unsigned int integer, fractional;
						
						angle = motor.steps / 11.3777777777778f;
						integer = (int) angle; //conversion from float to uint8_t
						angle = (angle - integer) * 1000.0f;
						fractional = (int) angle;
						
						if(motor.steps==4160)
						sprintf(lcd_buff,"\001\x80\004\377Angle: infinity ");
						else if(motor.steps==0)
						sprintf(lcd_buff,"\001\x80\004\377Angle: none     ");
						else
						sprintf(lcd_buff,"\001\x80\004\377Angle: %.3u.%.3u  ",integer,fractional);
						lcd_buff_full=1;
						
						motor.temporary_steps = motor.steps;
					}
					break;
				}
			}
		}
		break;
		
		case 2:
		{
			static uint8_t i=0;
			if(motor.temporary_steps==0)
			{
				PORTD = 0x00;
				sprintf(lcd_buff,"\001\x80\004\377%s: Off  ",txt11);
				lcd_buff_full=1;
			}
			else
			{
				if(motor.temporary_slowdown == 0)
				{
					if(motor.temporary_steps<4160)
					{
						if(motor.temporary_steps)
						{
							PORTD = pgm_read_byte(&moves[i]);
							motor.temporary_steps--;
						}
					}
					else if(motor.steps==4160)//infinity
					{
						PORTD = pgm_read_byte(&moves[i]);
					}
					//again wait
					motor.temporary_slowdown = motor.slowdown;
					
					if(motor.direction) //CW or CCW - motor direction
					i=(i+1)%8;
					else
					i=(i+7)%8;
					
					sprintf(lcd_buff,"\001\x80\004\377%s: On   ",txt11);
					lcd_buff_full=1;
				}
				else if(motor.temporary_slowdown>0)
				motor.temporary_slowdown--;
			}
		}
		break;
	}
}