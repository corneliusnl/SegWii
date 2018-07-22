#ifndef __SEGWII_H__
#define __SEGWII_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 7th, 2008
 */

#define F_CPU		16000000UL
//#define F_CPU		14318180UL

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "ioports.h"
#include "adc.h"

#define false	0
#define true	1 
	
#define SEGWII_VERSION	"0.1.0"

// set this to 1 to reduce Uart speed to 9600 (default for BT module) and to program the
// bluetooth module to the appropriate settings
#define BLUETOOTH_UNCONFIGURED	0

typedef uint8_t	bool;

typedef enum {
	FAIL,
	SUCCESS,
	TIMEOUT,
} ReturnType;

#define SEGWII_ADC_ANGTEMP_CH	ADC_CHANNEL_1
#define SEGWII_ADC_ANGRATE_CH	ADC_CHANNEL_2
#define SEGWII_ADC_BATTERY_CH	ADC_CHANNEL_3

#define TIME_TICKS_PER_SEC	64

#define MS_DELAY	(F_CPU/4000UL)
#define US_DELAY	(F_CPU/4000000UL)

#endif
