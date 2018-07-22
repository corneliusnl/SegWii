/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 7th, 2008
 * Content: timers
 */
#include "segwii.h"

void Timer1_SetupPWM(void);

/**
 * Initialize the timers
 */
void Timers_Init(void)
{
	
	// setup timer0
	TCCR0 = (0x05 << CS00);      	// Set TIMER0 prescaler to CLK/1024 
	TCNT0 = 256 - (F_CPU/TIME_TICKS_PER_SEC/1024);
	
	// Initialize timer interrupt
	TIMSK = (1 << OCIE0);//(1 << TOIE0);
}

/**
 * Configure 16-bit Timer1 for PWM
 */
void Timer1_SetupPWM(void)
{
	/*
	 * 10-bit phase-correct PWM (WGM10 and WGM11 bits), with OC1A OC1B used as PWM output.
	 * OC1A & OC1B will be set when up-counting, and cleared when down-counting (COM1A1|COM1A0).
	 */
	TCCR1A = (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10);

	/* 
	 * 16MHz / 1 / 1024 / 2 = 7800kHz
	 * prescalar divide by 1
	 */
	TCCR1B = (1 << CS10);
	
	OCR1A = OCR1B = 0;			// set PWM value to 0 
}

/**
 * Set duty of timer1a PWM 
 */
void Timer1_SetDutyA(int16_t duty)
{
	OCR1A = duty;
}

/**
 * Set duty of timer1b PWM
 */
void Timer1_SetDutyB(int16_t duty)
{
	OCR1B = duty;
}

