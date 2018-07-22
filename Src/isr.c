/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Feb 16th, 2008
 * Content: Interrupt Service Routines
 */

#include "segwii.h"
#include "uart.h"
#include "adc.h"
#include "time.h"
#include "i2c.h"
//#include "motors.h"

/*
 *	Default interrupt vector.
 */
ISR(__vector_default)
{
}

/*
 * ADC conversion complete
 */
ISR(ADC_vect)
{
	ADC_InterruptHandler();
}


/* 
 * Global timer ISR
 */
ISR(TIMER0_COMP_vect)
{
	Time_InterruptHandler();
	
	// reset the watch dog timer
	wdt_reset();
}


/*
 * Serial receive ISR
 */
ISR(USART_RXC_vect)
{
	Uart_InterruptHandler();
}

/**
 * Right motor encoder interrupt
 */
ISR(INT0_vect) 
{	
//	Motors_InterruptHandler();
}

/**
 * Left motor encoder interrupt
 */
ISR(INT1_vect) 
{
//	Motors_InterruptHandler();
}

/**
 * I2C Interrupt (Nunchuk)
 */
ISR(TWI_vect)
{
	I2C_InterruptHandler();
}
