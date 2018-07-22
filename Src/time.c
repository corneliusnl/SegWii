/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Martch 8th, 2008
 * Content: time routines
 */ 
#include "segwii.h"
#include "time.h"

volatile Clock	CurTime;
volatile uint32_t time_ticks = 0;


/**
 * Initialize time management
 */ 
void Time_Init(void)
{
	memset((void *)&CurTime, 0, sizeof(CurTime));
	time_ticks = 0;
}

/** 
 * Get time ticks
 */
uint32_t Time_GetTicks(void)
{
	return time_ticks;
}

/**
 * Get the current time
 */
Clock Time_GetCurrentTime(void)
{
	return CurTime;
}

/**
 * Handle timer ticks
 */
inline void Time_InterruptHandler(void)
{
	time_ticks++;
	
	if((time_ticks % TIME_TICKS_PER_SEC) == 0)
	{
		CurTime.second++;
		
		if(CurTime.second >= 60)
		{
			CurTime.second = 0;
			CurTime.minute++;
			
			if(CurTime.minute >= 60)
			{
				CurTime.minute = 0;
				CurTime.hour++;
			}
		}
	}
}

/*
 * Delay for X milliseconds
 */
void Time_DelayMS(uint16_t ms)
{
	while(ms-- > 0)
		_delay_loop_2(MS_DELAY);
}

/**
 * Delay for X microseconds
 */
void Time_DelayUS(uint16_t us)
{
	while(us-- > 0)
		_delay_loop_2(US_DELAY);
}

