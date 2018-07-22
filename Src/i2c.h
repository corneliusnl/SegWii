#ifndef __SEGWII_I2C_H__
#define __SEGWII_I2C_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 9th, 2008
 * Content: i2c (TWI) routines
 */

//#define I2C_SPEED	400000UL
//#define I2C_TWBR	((F_CPU / I2C_SPEED - 16) / 2);

#define I2C_MASTER	1
#define I2C_SLAVE	2

// status register
#define I2C_TWSR_PS1		0x00
#define I2C_TWSR_PS4		0x01
#define	I2C_TWSR_PS16		0x02
#define I2C_TWSR_PS64		0x03

#define I2C_BUFFER_SIZE		32
#define I2C_RX_BUFFER_SIZE	0
#define I2C_TX_BUFFER_SIZE	0

#define I2C_TIMEOUT		(TIME_TICKS_PER_SEC/4)

typedef enum
{
	READY,
	MASTER_RX,
	MASTER_TX,
	SLAVE_RX,
	SLAVE_TX,
} I2C_State;

typedef enum
{
	I2C_BLOCKING,
	I2C_CALLBACK,
} I2C_Mode;

void		I2C_Init(uint8_t type, uint32_t speed); 
int8_t		I2C_Start(void);
ReturnType	I2C_ReadBytes(uint8_t addr, uint8_t *data, uint8_t count);
ReturnType	I2C_WriteBytes(uint8_t addr, uint8_t *data, uint8_t count);
void		I2C_Stop(void);
ReturnType	I2C_Send(uint8_t data);
inline void	I2C_InterruptHandler(void);
void		I2C_Reply(uint8_t ack);
void		I2C_ReleaseBus(void);
void		I2C_SetMode(I2C_Mode mode);
I2C_Mode	I2C_GetMode(void);
void		I2C_SetReadCallback(void (*)(uint8_t *, bool));
void		I2C_SetWriteCallback(void (*)(bool));
 
#endif
