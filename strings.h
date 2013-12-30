#ifndef STRINGS_H
#define STRINGS_H

#include <avr/pgmspace.h> //progmem
// -- wait \004\377 -- command \001\x28 --

const char lcd_init[] PROGMEM = "\004\377\001\x28\004\377\001\x28\004\377\001\x28\004\377\001\x0c\004\377\001\x06\004\377\001\x01\004\377";

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

const char txt_hours[] PROGMEM = "\001\x80\004\xff%S:%c%.2d:%.2d:%.2d\001\xc0\004\xff%s %.2d.%.2d.%.4d";
const char txt_minutes[] PROGMEM = "\001\x80\004\xff%S: %.2d%c%.2d:%.2d\001\xc0\004\xff%s %.2d.%.2d.%.4d";
const char txt_seconds[] PROGMEM = "\001\x80\004\xff%S: %.2d:%.2d%c%.2d\001\xc0\004\xff%s %.2d.%.2d.%.4d";
const char txt_day_week[] PROGMEM = "\001\x80\004\xff%S: %.2d:%.2d:%.2d\001\xc0\004\xff%s%c%.2d.%.2d.%.4d";
const char txt_day_month[] PROGMEM = "\001\x80\004\xff%S: %.2d:%.2d:%.2d\001\xc0\004\xff%s %.2d%c%.2d.%.4d";
const char txt_month[] PROGMEM = "\001\x80\004\xff%S: %.2d:%.2d:%.2d\001\xc0\004\xff%s %.2d.%.2d%c%.4d";
const char txt_year[] PROGMEM = "\001\x80\004\xff%S: %.2d:%.2d:%.2d\001\xc0\004\xff%s %.2d.%.2d.%.4d%c";
const char txt_show_time[] PROGMEM = "\001\x80\004\xff%S: %.2d:%.2d:%.2d\001\xc0\004\xff%s %.2d.%.2d.%.4d ";
const char txt_volt_off[] PROGMEM = "\001\x80\004\377%S       \001\xc0\004\377%.2d.%.3d V (off)  ";
const char txt_volt_on[] PROGMEM = "\001\x80\004\377%S       \001\xc0\004\377%.2d.%.3d V (on)   ";
const char txt_therm_off[] PROGMEM = "\001\x80\004\377%S     \001\xc0\004\377%.2d.%.3d C (off)  ";
const char txt_therm_on[] PROGMEM = "\001\x80\004\377%S     \001\xc0\004\377%.2d.%.3d C (on)   ";
const char txt_dis_off[] PROGMEM = "\001\x80\004\377%S (cm)\001\xc0\004\377%.3d.%.3d (off)   ";
const char txt_dis_on[] PROGMEM = "\001\x80\004\377%S (cm)\001\xc0\004\377%.3d.%.2d (on)     ";
const char txt_step_slow_none[] PROGMEM = "\001\x80\004\377Slowdown: none  ";
const char txt_step_slow[] PROGMEM = "\001\x80\004\377Slowdown: %ux    ";
const char txt_step_cw[] PROGMEM = "\001\x80\004\377Direction: CW   ";
const char txt_step_ccw[] PROGMEM = "\001\x80\004\377Direction: CCW  ";
const char txt_step_ang_inf[] PROGMEM = "\001\x80\004\377Angle: infinity ";
const char txt_step_ang_none[] PROGMEM = "\001\x80\004\377Angle: none     ";
const char txt_step_ang[] PROGMEM = "\001\x80\004\377Angle: %.3u.%.3u  ";
const char txt_step_off[] PROGMEM = "\001\x80\004\377%S: Off  ";
const char txt_step_on[] PROGMEM = "\001\x80\004\377%S: On   ";

#endif