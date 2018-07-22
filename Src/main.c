/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Feb 16th, 2008
 * Content: program entrypoint
 */
#include "segwii.h"
#include "bluetooth.h"
#include "uart.h"
#include "timers.h"
#include "time.h"
#include "support.h"
#include "adc.h"
#include "nunchuk.h"
#include "debug.h"
#include "motors.h"

extern char resp[];

const char bluetooth_name[] PROGMEM = "SegWii-Brain";

static const uint8_t adc_pattern[] = {SEGWII_ADC_ANGRATE_CH, SEGWII_ADC_ANGRATE_CH, SEGWII_ADC_ANGRATE_CH, SEGWII_ADC_BATTERY_CH};
#define ADC_PATTERN_COUNT (sizeof(adc_pattern)/sizeof(uint8_t))

/*
 * local prototypes
 */
int 			GetChar(FILE *file);
int 			PutChar(char c, FILE *file);

FILE mystdout = FDEV_SETUP_STREAM(PutChar, GetChar, _FDEV_SETUP_RW);

/**
 * Description:
 *	Transmits a character on UART0.
 *
 * Inputs:	
 * 	c -- the character to send	
 * 
 * Returns: 	
 *	1, needed for compatibility with the AVR libc
 */
int PutChar(char c, FILE *file)
{
	Uart_PutChar(UART0, c);
	return 0;
}


/**
 * Description:
 *	Reads a character from UART0.  Non-blocking.
 *
 * Returns: 	
 *	The received character, as an int
 */
int GetChar(FILE *file)
{
	int ch;
	
	Uart_GetChar((uint8_t *)&ch);
	
	return ch;
}

/**
 * Program entrypoint
 */
int main(void)
{
	// initialize stdio (avoiding malloc madness)
	stdout = stdin = &mystdout;
		
	wdt_disable();			// disable watchdog
	Bluetooth_Init();
	Timers_Init();
	Timer1_SetupPWM();
	Time_Init();
	ADC_Init();
	Gyro_Init();
	Motors_Init();
	sei();					// enable interrupts
	Nunchuk_Init();			// requires interrupts
	
#if _DEBUG_
	Debug_Init();
#endif
		
	wdt_enable(WDTO_2S);	// set watchdog for 2s
	
#if BLUETOOTH_UNCONFIGURED
	if((Bluetooth_SetSpeed(BT_HIGH_SPEED) == SUCCESS) &&
		(Bluetooth_SetName(bluetooth_name) == SUCCESS) &&
		(Bluetooth_SetConfigFlag(BT_CFG_CONN_ACTIVE_GPIO) == SUCCESS))
	{
		Bluetooth_SaveToFlash();
	}
#endif

	printf_P(PSTR("Segwii v%s\r\n"), SEGWII_VERSION);
	Uart_Process();
	
	//Time_DelayMS(1500);
	
	//Nunchuk_StartStreaming();
	ADC_StartStreaming(adc_pattern, ADC_PATTERN_COUNT);
	
	while(1)
	{		
		//Nunchuk_PollDevice();
		
		//wdt_reset();

		//Gyro_PerformAnalysis();
		//Nunchuk_PerformAnalysis();
		//Balance_Perform();
		
#if _DEBUG_
		Debug_Process();
#endif

#if BT_DEBUG_CMDMODE
		Uart_SendString(resp);
#endif
		Uart_Process();

		//Time_DelayMS(300);
	}
	
	return 0;
}

