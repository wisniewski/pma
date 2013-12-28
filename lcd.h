#ifndef LCD_H
#define LCD_H

extern uint8_t volatile lcd_buff_full;
extern char *lcd_buff;

struct lcd_field //definicja struktury <-------- P O L A  B I T O W E
{
	volatile char rs : 1; //zmienna nie podlegajaca procesowi optymalizacji
	volatile char rw : 1;
	volatile char e : 1;
	volatile char buz: 1; //odstep bo trzeciego bitu nie uzywamy
	volatile uint8_t data : 4; //dostep do d4-d7 w LCD
};

#define lcd_rs ((*((struct lcd_field *) &PORTB)).rs) //0 lub 1 na pinie
#define lcd_rs_dir (((struct lcd_field *) &DDRB) -> rs)
//operator wyluskania pola -> jesli mamy wskaznik do struktury, INICJALIZACJA

#define lcd_rw ((*((struct lcd_field *) &PORTB)).rw) //0 lub 1 na pinie
#define lcd_rw_dir (((struct lcd_field *) &DDRB) -> rw) //inicjalizacja

#define lcd_e ((*((struct lcd_field *) &PORTB)).e) //0 lub 1 na pinie
#define lcd_e_dir (((struct lcd_field *) &DDRB) -> e) //inicjalizacja

#define buzzer ((*((struct lcd_field *) &PORTB)).buz) //0 lub 1 na pinie
#define buzzer_dir (((struct lcd_field *) &DDRB) -> buz) //inicjalizacja

#define lcd_data ((*((struct lcd_field *) &PORTD)).data) //0 lub 1 na pinie
#define lcd_data_dir (((struct lcd_field *) &DDRD) -> data) //inicjalizacja

//deklaracje uzytych funkcji
void lcd_initiation (char data, char rs);
void lcd_write_data(char data);
void lcd_write_command(char data);
void lcd_show(void);
void lcd_clear(void);

#endif