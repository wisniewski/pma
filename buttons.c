﻿#include "buttons.h"

void buttons_debouncing(void)
{
	static uint8_t tmp_key, key_cnt, t;
	if(key_cnt == 0)
	{
		switch(t)
		{
			case 0:
			{
				tmp_key = hardware_keys;
				if(tmp_key != 15) //if is not 0xf
				{
					t = 1;
					key_cnt = 200;
				}
			}
			break;

			case 1:
			{
				if(hardware_keys == tmp_key)
				{
					keys = (~tmp_key)&0xf; //clear 4 MSB
					t = 2;
				}
				else
				t = 0;
			}
			break;

			case 2:
			{
				if(hardware_keys == 15)
				t = 0;
			}
			break;
		}
	}
	else
	key_cnt--;
}