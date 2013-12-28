#ifndef I2C_H
#define I2C_H

#include<avr/io.h>
#include <util/twi.h>

#define ADRW_ACK 18
#define DATA_ACK 28
#define M41T00 208

#define REG_SECONDS 0
#define REG_MINUTES 1
#define REG_HOURS 2
#define REG_DAY_OF_THE_WEEK 3
#define REG_DAY_OF_THE_MONTH 4
#define REG_MONTH 5
#define REG_YEAR 6

#define ACK 1
#define NACK 0

static inline void I2C_wait_for_complete(void) {while (!(TWCR & (1<<TWINT)));};

uint8_t bcd2dec(uint8_t i);
uint8_t dec2bcd(uint8_t i);

void I2C_initiation(void);
uint8_t I2C_start(void);
void I2C_stop(void);
uint8_t I2C_send_address(uint8_t address);
void I2C_start_and_select_address(uint8_t addr);
uint8_t I2C_send_data(uint8_t data);
uint8_t I2C_receive_data(uint8_t ack);

void I2C_write_value(uint8_t register, uint8_t value);
uint8_t I2C_get_value(uint8_t address, uint8_t nack_ack);

extern uint8_t address;

#endif