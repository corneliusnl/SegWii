/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Feb 16th, 2008
 * Content: uart routines
 */ 
#include "segwii.h"
#include "time.h"
#include "uart.h"
#include "support.h"

#define MEM_REG(a)	(uint8_t *)_SFR_MEM_ADDR(a)
#define IO_REG(a)	(uint8_t *)_SFR_IO_ADDR(a)

// receive buffer
static volatile uint8_t		Uart_RxBuf[UART_RX_BUF_SIZE];
static volatile uint16_t	Uart_RxHeadPtr = 0;						// UART receive buffer read pointer
static volatile uint16_t	Uart_RxTailPtr = 0;						// UART receive buffer write pointer
static volatile uint16_t	Uart_RxBufCount = 0;

// transmit buffer
static volatile uint8_t		Uart_TxBuf[UART_TX_BUF_SIZE];
static volatile uint16_t	Uart_TxHeadPtr = 0;						// UART transmit buffer read pointer
static volatile uint16_t	Uart_TxTailPtr = 0;						// UART transmit buffer write pointer
static volatile uint16_t	Uart_TxBufCount = 0;

UartType uart[] = 
{
	//{UBRRH, UBRRL, UCSRA, UCSRB, UCSRC, UDR},
#if defined(__AVR_ATmega128__)
	{IO_REG(UBBR0H), MEM_REG(UBBR0L), MEM_REG(UCSR0A), MEM_REG(UCSR0B), MEM_REG(UCSR0C), MEM_REG(UDR0)},
	{IO_REG(UBBR1H), MEM_REG(UBBR1L), MEM_REG(UCSR1A), MEM_REG(UCSR1B), MEM_REG(UCSR1C), MEM_REG(UDR1)},
#elif defined(__AVR_ATmega32__)
	{MEM_REG(UBRRH), MEM_REG(UBRRL), MEM_REG(UCSRA), MEM_REG(UCSRB), MEM_REG(UCSRC), MEM_REG(UDR)},
#endif
};

/*
 * Initialize UART
 */
void Uart_Init(uint8_t uartPtr, uint32_t speed)
{
	Uart_SetSpeed(uartPtr, speed);
	
	// Enable UART receiver and transmitter and receive interrupt
	*(uart[uartPtr].mUCSRB) 		= (1 << RXEN) | (1 << TXEN) | (1 << RXCIE); 
	
	// Set frame format: 8 data 1 stop
#if defined(__AVR_ATmega32__)
	*(uart[uartPtr].mUCSRC) 		=  (1 << URSEL) | (3 << UCSZ0);
#elif defined(__AVR_ATmega128__)
	*(uart[uartPtr].mUCSRC) 		=  (3 << UCSZ0);
#endif

	Uart_FlushTx();
	Uart_FlushRx();
}

/**
 * Set the speed of the uart
 */
void Uart_SetSpeed(uint8_t uartPtr, uint32_t speed)
{
	uint16_t ubrr;
	
	ubrr = (F_CPU/16/speed - 1);
	
	// Set the baud rate
	*(uart[uartPtr].mUBRR_H) 	= (uint8_t)(ubrr >> 8);                  
	*(uart[uartPtr].mUBRR_L) 	= (uint8_t)(ubrr);
}

/**
 * Perform buffered transmission
 */
void Uart_Process(void)
{
//	EnterCritical();		
	
	while(Uart_TxBufCount != 0)
	{
		// wait for empty transmit buffer
		loop_until_bit_is_set(*(uart[UART0].mUCSRA), UDRE);
		
		// copy data to transmit buffer
		*(uart[UART0].mUDR) = Uart_TxBuf[Uart_TxTailPtr];
		
		Uart_TxTailPtr = (Uart_TxTailPtr + 1) % UART_TX_BUF_SIZE;
		Uart_TxBufCount--;
	}
	
//	ExitCritical();
}

/*
 * Tranmit a single byte
 */
void Uart_PutChar(uint8_t uartPtr, uint8_t c)
{
	Uart_TxEnqueue(c);
}

/**
 * Return a character from Uart buffer (if available)
 */
ReturnType Uart_GetChar(char *c)
{
	uint16_t time 		= 0;
	bool timeout		= false;
	ReturnType ret 		= FAIL;
 
/*	time = Time_GetTicks();
	while(!Uart_RxBufHasData() && !timeout) 
	{
		if((Time_GetTicks() - time) > UART_TIMEOUT)
		{
			timeout = true;
		}
	}
*/
	*c = 0; 
	
	if(!timeout)
	{
	
		if(Uart_RxBufCount > 0)
		{	
			EnterCritical();
			*c = Uart_RxBuf[Uart_RxTailPtr];
			Uart_RxTailPtr = (Uart_RxTailPtr + 1) % UART_RX_BUF_SIZE;
			Uart_RxBufCount--;
			ExitCritical();
			
			ret = SUCCESS;
		}
		else
			ret = FAIL;
	}
	else
	{
		ret = TIMEOUT;
	}
	
	return ret;
}

/**
 * Returns TRUE if data is available
 */
bool Uart_DataAvailable(uint8_t uartPtr)
{
	if(*(uart[uartPtr].mUCSRA) & (1 << RXC))
	{
		return true;
	}
	
	return false;
}

/**
 * Flush Uart receive buffer
 */
void Uart_FlushRx(void)
{
	EnterCritical();
	Uart_RxHeadPtr 		= 0;
	Uart_RxTailPtr 		= 0;
	Uart_RxBufCount 	= 0;
	ExitCritical();
}

/**
 * Flush Uart transmit buffer
 */
void Uart_FlushTx(void)
{
	EnterCritical();
	Uart_TxHeadPtr 		= 0;
	Uart_TxTailPtr 		= 0;
	Uart_TxBufCount 	= 0;
	ExitCritical();
}


/**
 * Enqueue byte into transmit buffer
 */
ReturnType Uart_TxEnqueue(uint8_t data)
{
	ReturnType ret 		= FAIL;
	
	//EnterCritical();
	
	if(Uart_TxBufCount < UART_TX_BUF_SIZE)
	{
		Uart_TxBuf[Uart_TxHeadPtr] = data;
		Uart_TxHeadPtr = (Uart_TxHeadPtr + 1) % UART_TX_BUF_SIZE;
		Uart_TxBufCount++;	
		
		ret = SUCCESS;
	}
	
	//ExitCritical();
	
	return ret;
}

/**
 * Enqueue byte into receive buffer
 */
ReturnType Uart_RxEnqueue(uint8_t data)
{
	ReturnType ret 		= FAIL;
	
	EnterCritical();
	
	if(Uart_RxBufCount < UART_RX_BUF_SIZE)
	{
		Uart_RxBuf[Uart_RxHeadPtr] = data;
		Uart_RxHeadPtr = (Uart_RxHeadPtr + 1) % UART_RX_BUF_SIZE;
		Uart_RxBufCount++;	
		
		ret = SUCCESS;
	}
	
	ExitCritical();
	
	return ret;
}

/**
 * Does receive buffer contain
 */
bool Uart_RxBufHasData(void)
{	
	bool ret 	= false;
	
	EnterCritical();
	
	if(Uart_RxBufCount > 0)
	{
		ret = true;
	}
	
	ExitCritical();
	
	
	return ret;
}

/**
 * Send a string over uart (blocking)
 */
void Uart_SendString(char *data)
{
	while(*data != '\0')
	{
		// wait for empty transmit buffer
		loop_until_bit_is_set(*(uart[UART0].mUCSRA), UDRE);
		
		// copy data to transmit buffer
		*(uart[UART0].mUDR) = *data; 	
		data++;
	}
}

/**
 * Get receive data
 */
void Uart_ReceiveString(char *data, uint8_t len)
{
	while((len-- > 0) && (Uart_GetChar(data++) == SUCCESS));
	*data = '\0';
}


/**
 * Handle interrupt routine
 */
inline void Uart_InterruptHandler(void)
{
	Uart_RxEnqueue((uint8_t)*(uart[UART0].mUDR));
}



