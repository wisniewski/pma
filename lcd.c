#include <avr/pgmspace.h> //progmem
#include <stdlib.h> //malloc
#include <stdio.h> //sprintf
#include "lcd.h"

void lcd_initiation (char data, char rs)
{
	lcd_rw = 0;
	lcd_rs = rs;
	lcd_e = 1;
	lcd_data = data >> 4; //przesuniecie na 4 najmlodsze bity
	lcd_e = 0;
	lcd_e = 1;
	lcd_data = data & 0xF;
	lcd_e = 0;
	//gdyby nie pola bitowe... (((((PORTD >> 4)++) & 0xF) << 2) | PORTD & 0xC3)
}

void lcd_write_data(char data)
{
	lcd_initiation(data, 1); //1 bo RS 1 to data
}

void lcd_write_command(char data)
{
	lcd_initiation(data, 0); //0 to command
}

void lcd_show(void)
{
	static uint8_t lcd_cnt, lcd_read;
	if((lcd_buff_full) && (lcd_cnt == 0))
	{
		switch(lcd_buff[(int)lcd_read])
		{
			case 0: //koniec lancucha
			{
				lcd_buff_full = 0;
				lcd_read = 0;
			}
			break;
			case 1: //wyslanie komendy do wyswietlacza
			{
				lcd_read++;
				lcd_write_command(lcd_buff[lcd_read++]); //wysylam komende
				//i znowu inkrementuje, aby wskazywac na kolejny element
			}
			break;
			case 4: //czekanie
			{
				lcd_read++;
				lcd_cnt = lcd_buff[lcd_read++];
			}
			break;
			//lcd_read mowi ktory znak odczytac oraz inkrementacja do nastepnego znaku
			default: lcd_write_data(lcd_buff[lcd_read++]); break;
		}
	}
	else if(lcd_cnt > 0)
	lcd_cnt--;
}