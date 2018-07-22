#ifndef __SEGWII_UART_H__
#define __SEGWII_UART_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 */ 
 
#define UART0					(0)
#define UART1					(1)

#define UART_RX_BUF_SIZE		64
#define UART_TX_BUF_SIZE		250

#define UART_TIMEOUT	(TIME_TICKS_PER_SEC/10)

typedef struct 
{
	uint8_t 	*mUBRR_H;
	uint8_t 	*mUBRR_L;
	uint8_t 	*mUCSRA;
	uint8_t 	*mUCSRB;
	uint8_t 	*mUCSRC;
	uint8_t 	*mUDR;
} UartType;


void		Uart_Init(uint8_t uartPtr, uint32_t speed);
void		Uart_SetSpeed(uint8_t uartPtr, uint32_t speed);
ReturnType	Uart_GetChar(char *c);
void		Uart_PutChar(uint8_t uartPtr, uint8_t c);
bool		Uart_DataAvailable(uint8_t uartPtr);
void		Uart_SendString(char *data);
void		Uart_ReceiveString(char *data, uint8_t len);
void		Uart_FlushTx(void);
void		Uart_FlushRx(void);
void		Uart_Process(void);
ReturnType	Uart_RxEnqueue(uint8_t data);
ReturnType	Uart_TxEnqueue(uint8_t data);
bool		Uart_RxBufHasData(void);
inline void Uart_InterruptHandler(void);


#endif 
