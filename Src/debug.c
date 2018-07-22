/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Mar 16th, 2008
 * Content: debug/testing routines
 */
#include <string.h>
#include "segwii.h"
#include "adc.h"
#include "nunchuk.h"
#include "debug.h"
#include "uart.h"
#include "motors.h"
#include "time.h"
#include "timers.h"
#include "bluetooth.h"
#include "gyro.h"

extern char bluetooth_name[];

#if _DEBUG_


char *Debug_GetCommand(void);


/**
 * Initialize debug support
 */
void Debug_Init(void)
{
	
}

/**
 * Process input for debug query
 */
void Debug_Process(void)
{
	char *cmd;
	
	cmd = Debug_GetCommand();
	if(cmd != NULL)
	{
		printf_P(PSTR("Cmd: %s\r\n"), cmd);
		
		if(strstr_P(cmd, PSTR("battery")))
		{		
			printf_P(PSTR("Battery: %d\r\n"), ADC_GetSingleSample(SEGWII_ADC_BATTERY_CH));
		}
		else if(strstr_P(cmd, PSTR("ang_temp")))
		{
			printf_P(PSTR("AngTemp: %d\r\n"), ADC_GetSingleSample(SEGWII_ADC_ANGTEMP_CH));
		}
		else if(strstr_P(cmd, PSTR("ang_rate")))
		{
			printf_P(PSTR("AngRate: %d\r\n"), ADC_GetSingleSample(SEGWII_ADC_ANGRATE_CH));
		}
		else if(strstr_P(cmd, PSTR("gyro_analyze")))
		{
			Gyro_PerformAnalysis();
		}
		else if(strstr_P(cmd, PSTR("time")))
		{
//			Clock time = Time_GetCurrentTime();
			printf_P(PSTR("Time is %2d:%02d:%02d\r\n"), CurTime.hour, CurTime.minute, CurTime.second);		
		}
		else if(strstr_P(cmd, PSTR("nunchuk")))
		{
			Nunchuk_PrintData(Nunchuk_PollDevice());
		}
		else if(strstr_P(cmd, PSTR("nc_analyze")))
		{
			Nunchuk_PerformAnalysis();
		}
		else if(strstr_P(cmd, PSTR("wiicali")))
		{
			Nunchuk_ReadCalibration();
		}
		else if(strstr_P(cmd, PSTR("dirs")))
		{
			Motors_Enable(MtrBoth);
		}
		else if(strstr_P(cmd, PSTR("motors")))
		{
			Motors_SetDutyL(30);
			//Motors_SetDutyR(20);
			
			Motors_Enable(MtrBoth);
			
			Motors_Setup(DirForward);
	//		Time_DelayMS(2000);
		//	Motors_Enable(MtrLeft);
		//	Time_DelayMS(2000);
			
			Motors_Setup(DirReverse);
			Motors_Enable(MtrLeft);
			Time_DelayMS(2000);
			
			Motors_SetDutyL(5);
			Time_DelayMS(1000);
			Motors_Enable(MtrNone);
		}
		else if(strstr_P(cmd, PSTR("getroll")))
		{
			float roll;
			roll = Nunchuk_GetRoll(Nunchuk_PollDevice());
			printf_P(PSTR("roll: %f\r\n"), roll);
		}
		else if(strstr_P(cmd, PSTR("getpitch")))
		{
			float pitch;
			Nunchuk_GetPitch(Nunchuk_PollDevice());
			printf_P(PSTR("pitch: %f\r\n"), pitch);
		}
		else if(strstr_P(cmd, PSTR("btconfig")))
		{
			if((Bluetooth_SetSpeed(BT_HIGH_SPEED) == SUCCESS) &&
				(Bluetooth_SetName(bluetooth_name) == SUCCESS) &&
				(Bluetooth_SetConfigFlag(BT_CFG_CONN_ACTIVE_GPIO) == SUCCESS))
			{
				Bluetooth_SaveToFlash();
			}
		}
	}
}

/**
 * Scan input terminating on newlines looking for commands
 */
char *Debug_GetCommand(void)
{
	static char cmd[80];
	static char *pcmd = NULL;
	static char command[80];
	
	// first start
	if(pcmd == NULL)
		pcmd = cmd;
		
	while(Uart_RxBufHasData() && (Uart_GetChar(pcmd) == SUCCESS))
	{
		if((*pcmd == '\r') || (*pcmd == '\n'))
		{
			// got complete command, null terminate and store
			*pcmd = '\0';
			strcpy(command, cmd);
					
			// reset cycle
			pcmd = cmd;
			*pcmd = '\0';
			
			return command;
		}
		
		pcmd++;
	}	
	
	return NULL;
}

#endif
