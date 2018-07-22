#ifndef __SEGWII_TIME_H__
#define __SEGWII_TIME_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Martch 8th, 2008
 */ 
 
typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} Clock;

extern volatile Clock	CurTime;
extern volatile uint32_t time_ticks;

#define Delay()	asm volatile ( \
	"nop \n\t" \
	"nop \n\t" \
	"nop \n\t" \
	"nop \n\t")
	

void		Time_Init(void);
uint32_t	Time_GetTicks(void);
inline void	Time_InterruptHandler(void);
Clock		Time_GetCurrentTime(void);
void		Time_DelayMS(const uint16_t ms);
void		Time_DelayUS(const uint16_t us);

#endif
