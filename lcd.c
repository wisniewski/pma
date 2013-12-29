#include "lcd.h"

void lcd_initiation (char data, char rs)
{
	lcd_rw = 0;
	lcd_rs = rs;
	lcd_e = 1;
	lcd_data = data >> 4; //move to LSB
	lcd_e = 0;
	lcd_e = 1;
	lcd_data = data & 0xF;
	lcd_e = 0;
	//without bit field... (((((PORTD >> 4)++) & 0xF) << 2) | PORTD & 0xC3)
}

void lcd_write_data(char data)
{
	lcd_initiation(data, 1); //1 - RS == data
}

void lcd_write_command(char data)
{
	lcd_initiation(data, 0); //0 - RS ==command
}

void lcd_show(void)
{
	static uint8_t lcd_cnt, lcd_read;
	if((lcd_buff_full) && (lcd_cnt == 0))
	{
		switch(lcd_buff[(int)lcd_read])
		{
			case 0: //end of string
			{
				lcd_buff_full = 0;
				lcd_read = 0;
			}
			break;
			case 1: //send command to driver
			{
				lcd_read++;
				lcd_write_command(lcd_buff[lcd_read++]); //send command and i++ to point next element
			}
			break;
			case 4: //waiting
			{
				lcd_read++;
				lcd_cnt = lcd_buff[lcd_read++];
			}
			break;
			default: lcd_write_data(lcd_buff[lcd_read++]); break;
		}
	}
	else if(lcd_cnt > 0)
	lcd_cnt--;
}