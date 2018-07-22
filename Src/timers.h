#ifndef __SEGWII_TIMERS_H__
#define __SEGWII_TIMERS_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 7th, 2008
 */

void Timers_Init(void);

void Timer1_SetupPWM(void);
void Timer1_SetDutyA(int16_t duty);
void Timer1_SetDutyB(int16_t duty); 

#endif
