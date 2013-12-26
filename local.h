#ifndef LOCAL_H
#define LOCAL_H

extern uint8_t volatile keys; //ktory przycisk nacisniety?
extern uint8_t volatile local;
extern uint8_t volatile lcd_buff_full;
extern char *lcd_buff;
extern volatile _Bool run_function;
extern unsigned int time[3];
extern unsigned int measurement;
extern uint8_t first_time[3];
extern unsigned int distance;

extern const uint8_t moves[8];

extern const char txt1[];
extern const char txt2[];
extern const char txt3[];
extern const char txt4[];
extern const char txt5[];
extern const char txt6[];
extern const char txt7[];
extern const char txt8[];
extern const char txt9[];
extern const char txt10[];
extern const char txt11[];
extern const char txt12[];

//deklaracje funkcji lokalnych
void func_menu11(char c);
void func_menu12(char c);
void func_menu1311(char c);
void func_menu1312(char c);
void func_menu21(char c);
void func_menu22(char c);
void func_menu23(char c);
void func_menu31(char c);

#endif