/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 9th, 2008
 * Content: i2c (TWI) routines
 */
#include <util/twi.h>
#include "segwii.h"
#include "time.h"
#include "i2c.h"
#include "support.h"
#include "uart.h"

#define DEBUG_I2C	0

// master buffer
static volatile uint8_t i2c_masterBuf[I2C_BUFFER_SIZE];
static volatile uint8_t i2c_masterBufIdx = 0;
static volatile uint8_t i2c_masterBufLen = 0;
// receiver buffer
static volatile uint8_t i2c_rxBuf[I2C_RX_BUFFER_SIZE];
static volatile uint8_t i2c_rxBufIdx = 0;
static volatile uint8_t i2c_rxBufLen = 0;
// transmit buffer
static volatile uint8_t i2c_txBuf[I2C_TX_BUFFER_SIZE];
static volatile uint8_t i2c_txBufIdx = 0;
static volatile uint8_t i2c_txBufLen = 0;

static volatile I2C_State i2c_state = READY;
static volatile uint8_t i2c_slarw;
static I2C_Mode i2c_mode = I2C_BLOCKING;

// Callback functions
void (*i2c_write_callback)(bool success);
void (*i2c_read_callback)(uint8_t *data, bool success);

/**
 * Initialize I2C bus
 */
void I2C_Init(uint8_t type, uint32_t speed)
{
	// Activate internal pullups of SCL and SDA pins
	PORTC = PORTC | 0x03;

	// initialize variables
	i2c_rxBufIdx = i2c_rxBufLen = 0;
	i2c_txBufIdx = i2c_txBufLen = 0;
	i2c_masterBufIdx = i2c_masterBufLen = 0;
	i2c_state = READY;
	i2c_mode = I2C_BLOCKING;
	
	i2c_read_callback = NULL;
	i2c_write_callback = NULL;
	
	// Initialize the TWI clock
	TWSR = I2C_TWSR_PS1;
	TWBR = ((F_CPU / speed) - 16) / 2;
	
	// Enable I2C with acks and interrupts
	TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA);
}

/**
 * Set operating mode
 */
void I2C_SetMode(I2C_Mode mode)
{
	i2c_mode = mode;
}

/**
 * Get operating mode 
 */
I2C_Mode I2C_GetMode(void)
{
	return i2c_mode;
}

/**
 * Set read callback function
 */
void I2C_SetReadCallback(void (*func)(uint8_t *, bool))
{
	i2c_read_callback = func;
}

/**
 * Set write callback function
 */
void I2C_SetWriteCallback(void (*func)(bool))
{
	i2c_write_callback = func;
}

/**
 * Start I2C communications
 */
int8_t I2C_Start(void)
{
	i2c_txBufIdx = i2c_txBufLen = 0;
	
//	printf("PC: %02X\r\n", PORTC);
	
	return 0;
}

/**
 * Read bytes from I2C
 */
ReturnType I2C_ReadBytes(uint8_t addr, uint8_t *data, uint8_t length)
{
	//ReturnType ret = FAIL;
	uint8_t i;
	//uint32_t time;

	if(length > I2C_BUFFER_SIZE)
		return FAIL;
	
	// wait till bus is free
	//time = Time_GetTicks();
	//while((i2c_state != READY) && ((Time_GetTicks() - time) < I2C_TIMEOUT));
	while(i2c_state != READY);
	i2c_state = MASTER_RX;

	i2c_masterBufIdx = 0;
	i2c_masterBufLen = length;
		
	// send read command
	i2c_slarw = TW_READ | (addr << 1);
	TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT) | (1 << TWSTA);
	
	if(i2c_mode == I2C_BLOCKING)
	{	
		// wait for operation to complete
		//time = Time_GetTicks();
		//while((i2c_state == MASTER_RX) && ((Time_GetTicks() - time) < I2C_TIMEOUT));
		while(i2c_state == MASTER_RX);
	
		// return data
		for(i = 0; i < length; i++)
		{
			data[i] = i2c_masterBuf[i];
#if DEBUG_I2C
			printf_P(PSTR("%02X "), data[i]);
#endif
		}
	}
	return SUCCESS;
}

/**
 * Write I2C
 */
ReturnType I2C_WriteBytes(uint8_t addr, uint8_t *data, uint8_t length)
{
	uint8_t i;
	//uint32_t time;

	// ensure data will fit into buffer
	if(length > I2C_BUFFER_SIZE)
		return FAIL;
		
		printf("hi1");
		Uart_Process();
	// wait until twi is ready, become master transmitter
//	time = Time_GetTicks();
//	while((i2c_state != READY) && ((Time_GetTicks() - time) < I2C_TIMEOUT));
	while(i2c_state != READY);
	i2c_state = MASTER_TX;
	
	// initialize buffer iteration vars
	i2c_masterBufIdx = 0;
	i2c_masterBufLen = length;
	
	// copy data to twi buffer
	for(i = 0; i < length; i++)
	{	
#if DEBUG_I2C
		printf_P(PSTR("%02X "), data[i]);
#endif

		i2c_masterBuf[i] = data[i];
	}
	
	// build sla+w, slave device address + w bit
	i2c_slarw = TW_WRITE | (addr << 1);
	
	// send start condition
	TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT) | (1 << TWSTA);
	
	// wait for write operation to complete (if blocking mode)
//	time = Time_GetTicks();
//	while((i2c_state == MASTER_TX) && ((Time_GetTicks() - time) < I2C_TIMEOUT));
	
	while((i2c_mode == I2C_BLOCKING) && (i2c_state == MASTER_TX));

	printf("hi2");
	return SUCCESS;
}

/**
 * Handle error
 */
void I2C_Stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWIE) | (1 << TWEA) | (1 << TWEN);
	
	// wait for stop condition to be executed, TWINT cleared after stop condition
	while(TWCR & (1 << TWSTO));
	
	i2c_state = READY;
}

/** 
 * Sends a byte or readys receive line
 */
void I2C_Reply(uint8_t ack)
{
	// transmit master read ready signal, with or without ack
	if(ack)
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
	else
		TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT);
}

/**
 * Release the I2C bus
 */
void I2C_ReleaseBus(void)
{
  // release bus
  TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT);
  
  i2c_state = READY;
}

/**
 * Handle i2c interrupts
 */
inline void I2C_InterruptHandler(void)
{
//#if DEBUG_I2C
	printf_P(PSTR("stat: %d\r\n"), TW_STATUS);
//#endif	

	switch(TW_STATUS)
	{
		/*
		 * All Master
		 */
		case TW_START:					// sent start condition
		case TW_REP_START:				// sent repeated start condition
			// copy device address and r/w bit to output register and ack
			TWDR = i2c_slarw;
			I2C_Reply(1);
			break;
			
		/*
		 * Master Transmitter
		 */
		case TW_MT_SLA_ACK:				// slave receiver acked address
		case TW_MT_DATA_ACK:			// slave receiver acked data
			// if there is data to send, send it, otherwise stop 
			if(i2c_masterBufIdx < i2c_masterBufLen)
			{
				// copy data to output register and ack
				TWDR = i2c_masterBuf[i2c_masterBufIdx++];
				I2C_Reply(1);
			}
			else
			{
				I2C_Stop();
				if(i2c_mode == I2C_CALLBACK)
					i2c_write_callback(true);
			}
			break;
			
		case TW_MT_SLA_NACK:			// address sent, nack received
		case TW_MT_DATA_NACK:			// data sent, nack received
			I2C_Stop();
			if(i2c_mode == I2C_CALLBACK)
				i2c_write_callback(true);
			break;
			
		case TW_MT_ARB_LOST:			// lost bus arbitration
			I2C_ReleaseBus();
			if(i2c_mode == I2C_CALLBACK)
				i2c_write_callback(true);
			break;
			
		/*
		 * Master Receiver
		 */
		case TW_MR_DATA_ACK:			// data received, ack sent
			i2c_masterBuf[i2c_masterBufIdx++] = TWDR;
			
		case TW_MR_SLA_ACK:				// address sent, ack received
			// ack if more bytes are expected, otherwise nack
			if(i2c_masterBufIdx < i2c_masterBufLen)
				I2C_Reply(1);
			else
			{
				I2C_Reply(0);
				if(i2c_mode == I2C_CALLBACK)
					i2c_read_callback(i2c_masterBuf, true);
			}
			break;
			
		case TW_MR_DATA_NACK:			// data received, nack sent
			i2c_masterBuf[i2c_masterBufIdx++] = TWDR;
			
		case TW_MR_SLA_NACK:			// address sent, nack received
			I2C_Stop();
			if(i2c_mode == I2C_CALLBACK)
				i2c_read_callback(i2c_masterBuf, false);
			break;
			
		/*
		 * Slave Receiver
		 *
		case TW_SR_SLA_ACK:				// addressed, returned ack
		case TW_SR_GCALL_ACK:			// addressed generally, returned ack
		case TW_SR_ARB_LOST_SLA_ACK:	// lost arbitration, returned ack
		case TW_SR_ARB_LOST_GCALL_ACK:	// lost arbitration, returned ack
			i2c_state = SLAVE_RX;
			
			// indicate that rx buffer can be overwritten and ack
			i2c_rxBufIdx = 0;
			I2C_Reply(1);
			break;
			
		case TW_SR_DATA_ACK:			// data received, returned ack
		case TW_SR_GCALL_DATA_ACK:		// data received generally, returned ack
			// if there is still room in the rx buffer
			if(i2c_rxBufIdx < I2C_BUFFER_SIZE)
			{
				// put byte in buffer and ack
				i2c_rxBuf[i2c_rxBufIdx++] = TWDR;
				I2C_Reply(1);
			}
			else
				// otherwise nack
				I2C_Reply(0);
			break;
			
		case TW_SR_STOP:				// stop or repeated start condition received
			// put a null char after data if there's room
			if(i2c_rxBufIdx < I2C_BUFFER_SIZE)
				i2c_rxBuf[i2c_rxBufIdx] = '\0';
			
			// callback to user defined callback
			//i2c_onSlaveReceive(twi_rxBuffer, twi_rxBufferIndex);
			
			// ack future responses
			I2C_Reply(1);
			
			// leave slave receiver state
			i2c_state = READY;
			break;
			
		case TW_SR_DATA_NACK:			// data received, returned nack
		case TW_SR_GCALL_DATA_NACK:		// data received generally, returned nack
			// nack back at master
			I2C_Reply(0);
			break;
			
		/*
		 * Slave Transmitter
		 *
		case TW_ST_SLA_ACK:				// addressed, returned ack
		case TW_ST_ARB_LOST_SLA_ACK:	// arbitration lost, returned ack
			i2c_state = SLAVE_TX;
			
			// ready the tx buffer index for iteration
			i2c_txBufIdx = 0;
			i2c_txBufLen = 0;
			
			// request for txBuffer to be filled and length to be set
			// note: user must call twi_transmit(bytes, length) to do this
			//twi_onSlaveTransmit();
			
			// if they didn't change buffer & length, initialize it
			if(i2c_txBufLen == 0)
			{
				i2c_txBufLen = 1;
				i2c_txBuf[0] = 0x00;
			}
			// transmit first byte from buffer, fall
		
		case TW_ST_DATA_ACK:			// byte sent, ack returned
			TWDR = i2c_txBuf[i2c_txBufIdx++];
			
			// if there is more to send, ack, otherwise nack
			if(i2c_txBufIdx < i2c_txBufLen)
				I2C_Reply(1);
			else
				I2C_Reply(0);
			break;
			
		case TW_ST_DATA_NACK:			// received nack, we are done 
		case TW_ST_LAST_DATA:			// received ack, but we are done already!
			// ack future responses
			I2C_Reply(1);
			
			// leave slave receiver state
			i2c_state = READY;
			break;
			
		/*
		 * All
		 */
		case TW_NO_INFO:				// no state information
			break;
			
		case TW_BUS_ERROR:				// bus error, illegal stop/start
			I2C_Stop();
			break;
	}
}
