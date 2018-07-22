#ifndef __SEGWII_SUPPORT_H__
#define __SEGWII_SUPPORT_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 */

#define MAX(a,b)	((a >= b) ? a : b)
#define MIN(a,b)	((a <= b) ? a : b)

/*#define EnterCritical()							asm volatile ( "in		__tmp_reg__, __SREG__" :: );	\
												asm volatile ( "cli" :: );								\
												asm volatile ( "push	__tmp_reg__" :: )

#define ExitCritical()							asm volatile ( "pop		__tmp_reg__" :: );				\
												asm volatile ( "out		__SREG__, __tmp_reg__" :: )
*/

#define IOSetPinLo(port,pin)	port &= ~(1 << pin)
#define IOSetPinHi(port,pin)	port |= (1 << pin)
#define IOSetPin(port,pin,val)	port = ((port & ~(1 << pin)) | (val << pin))
#define IOGetPin(port,pin)		((port & (1 << pin)) >> (pin))
#define IOSetPort(port,val)		port = val

#define EnterCritical()
#define ExitCritical()

#define MARKER(a)	Uart_PutChar(UART0, a)
//#define MARKER(a)
												
void	lpf_update(float *state, float tc, float interval, float input);
float	flim(float x, float lo, float hi);

#endif
