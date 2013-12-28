#include "i2c.h"

//-------------------------------------
// Initialize TWI
//-------------------------------------
void I2C_initiation(void) //init TWI Serial Interface
{
	TWCR = (1<<TWEA) | (1<<TWEN); //automatic acknowlege bit + enable twi - without interrupt bit
	TWBR = 100; //no prescaler in TWSR (1), freq: ~37 kHz
}

//-------------------------------------
// Generate START signal
//-------------------------------------
uint8_t I2C_start(void) //send starting signal
{
	TWCR = (1<<TWEN) | (1<<TWSTA) | (1<<TWINT); //enable twi, set start bit, interrupt when twi ends work
	I2C_wait_for_complete();
	
	if(TW_STATUS != TW_START)
	return 0; 
	else
	return 1;//if everything is OK - Status shout be equal Start
}

//-------------------------------------
// Generate STOP signal
//-------------------------------------
void I2C_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); //enable interrupt, enable twi and set stop bit
	while((TWCR & (1<<TWSTO))); //stop when stop bit is cleared
}

//-------------------------------------
// Write send address (slave id)
//-------------------------------------
uint8_t I2C_send_address(uint8_t address)
{
	uint8_t status;
	if((address & 0x01)==0) 
	status=TW_MT_SLA_ACK; //send
	else 
	status = TW_MR_SLA_ACK; //read

	TWDR = address; //load data into TWDR register
	TWCR = (1<<TWINT) | (1<<TWEN); //start transmission of data
	I2C_wait_for_complete();
	
	if(TW_STATUS!=status)
	return 0; //error
	else
	return 1;
}

//-------------------------------------
// Write start and select device (automatisation)
//-------------------------------------
void I2C_start_and_select_address(uint8_t addr)
{
	I2C_start();
	I2C_send_address(addr);
}

//-------------------------------------
// Send data to slave
//-------------------------------------
uint8_t I2C_send_data(uint8_t data)
{
	TWDR = data; //load data into TWDR register
	TWCR = (1<<TWINT) | (1<<TWEN); //start transmission of data
	I2C_wait_for_complete();

	if((TWSR == ADRW_ACK) | (TWSR == DATA_ACK))
	return 0;
	else
	return 1;
}

//-------------------------------------
// Receive data from slave
//-------------------------------------
uint8_t I2C_receive_data(uint8_t ack)
{
	TWCR = ack ? ((1 << TWINT) | (1 << TWEN) | (1 << TWEA)) : ((1 << TWINT) | (1 << TWEN)); //with or without ack
	I2C_wait_for_complete();
	return TWDR; 
}

uint8_t bcd2dec(uint8_t i)
{
	return (i & 0x0F) + 10 * ((i >> 4) & 0x0F);
}

uint8_t dec2bcd(uint8_t i)
{
	return (i % 10) + (((i / 10) % 10) << 4);
}

void I2C_write_value(uint8_t address, uint8_t value)
{
	I2C_start_and_select_address(M41T00);
	I2C_send_address(address);
	I2C_send_data(dec2bcd(value));
	I2C_stop();
}

uint8_t I2C_get_value(uint8_t address, uint8_t nack_ack)
{
	I2C_start_and_select_address(M41T00);
	I2C_send_address(address);
	I2C_start_and_select_address(0xd1);
	uint8_t temp = I2C_receive_data(nack_ack);
	I2C_stop();
	return bcd2dec(temp);
}



