/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Feb 16th, 2008
 * Content: bluetooth routines for BlueSMiRF module from SparkFun (WRL-08461)
 */
#include "segwii.h"
#include "bluetooth.h"
#include "uart.h"
#include "time.h"
 
#if 0
static const char reset_to_default[] PROGMEM	= "ATZ"; 
static const char rw_bt_pin[] PROGMEM			= "+BTPIN";
#endif

// Configuration strings
static const char bt_reset_to_factory[] PROGMEM	= "AT&F\r";
static const char bt_local_addr[] PROGMEM		= "AT+BTBDA\r";
static const char bt_save_to_flash[] PROGMEM	= "AT+BTFLS\r";
static const char bt_set_uart[] PROGMEM			= "AT+BTURT";
static const char bt_mode_server[] PROGMEM		= "AT+BTSRV=1\r";
static const char bt_mode_client[] PROGMEM		= "AT+BTCLT\r";
static const char bt_escape_seq[] PROGMEM		= "+++\r";
static const char bt_set_name[] PROGMEM			= "AT+BTLNM";
static const char bt_set_cfg[] PROGMEM			= "AT+BTCFG";

// Status responses
static const char bt_success[] PROGMEM = "\r\nOK\r\n";
static const char bt_failure[] PROGMEM = "\r\nERROR\r\n";

static char cmd_resp_buffer[BT_CMD_RESP_BUF_SIZE];

ReturnType Bluetooth_SendCommand(const char *command);

#if BT_DEBUG_CMDMODE
char bt_resp[256];  // debug util (blind, once entered into command mode)
#endif

/**
 * Initialize bluetooth device
 */
void Bluetooth_Init()
{
#if BLUETOOTH_UNCONFIGURED
	Uart_Init(UART0, BT_DEFAULT_SPEED);
#else
	Uart_Init(UART0, BT_HIGH_SPEED);
#endif

	// Set bluetooth name
	
	// Set higher baudrate
	//Bluetooth_SetSpeed(9600);
};

/**
 * Change UART speed
 */
ReturnType Bluetooth_SetSpeed(uint32_t speed)
{
	ReturnType ret = FAIL;
	char temp[60];
	
	Bluetooth_SetMode(Mode_Command);
	
	sprintf_P(temp, PSTR("%s=%ld,8,%d,1,%d\r"), bt_set_uart, speed, BT_UART_PAR_NONE, BT_UART_FLOW_RTSCTS); 
	if(Bluetooth_SendCommand(temp) == SUCCESS)
	{
		Uart_SetSpeed(UART0, speed);
		ret = SUCCESS;
	}

	Bluetooth_SetMode(Mode_Server);
	
	return ret;
};

/**
 * Set Mode to either Command or Data mode
 */
ReturnType Bluetooth_SetMode(BluetoothMode mode)
{	
	ReturnType ret = FAIL;
	
	if(mode == Mode_Command)
		ret = Bluetooth_SendCommand(bt_escape_seq);
	else if(mode == Mode_Server)
		ret = Bluetooth_SendCommand(bt_mode_server);
	else if(mode == Mode_Client)
		ret = Bluetooth_SendCommand(bt_mode_client);
			
	return ret;
}

/**
 * Reset to factory settings
 */
ReturnType Bluetooth_ResetToFactory(void)
{
	ReturnType ret;
	
	Bluetooth_SetMode(Mode_Command);
	
	ret = Bluetooth_SendCommand(bt_reset_to_factory);

	Bluetooth_SetMode(Mode_Server);

	return ret;
}

/**
 * Set configuration flag
 */
ReturnType Bluetooth_SetConfigFlag(uint8_t flag)
{
	ReturnType ret;
	char temp[60];
	
	Bluetooth_SetMode(Mode_Command);
	
	sprintf_P(temp, PSTR("%s=%d\r"), bt_set_cfg, flag); 
	ret = Bluetooth_SendCommand(temp);
	
	Bluetooth_SetMode(Mode_Server);
	
	return ret;
};


/**
 * Save settings to flash memory
 */ 
ReturnType Bluetooth_SaveToFlash(void)
{
	ReturnType ret;
	
	Bluetooth_SetMode(Mode_Command);
	
	ret = Bluetooth_SendCommand(bt_save_to_flash);

	Bluetooth_SetMode(Mode_Server);

	return ret;
}

/**
 * Set transceiver name
 */
ReturnType Bluetooth_SetName(const char *name)
{
	ReturnType ret;
	char temp[60];
	
	Bluetooth_SetMode(Mode_Command);
	
	sprintf_P(temp, PSTR("%s=\"%s\"\r"), bt_set_name, name);
	ret = Bluetooth_SendCommand(temp);
	
	Bluetooth_SetMode(Mode_Server);
	
	return ret;
}

/**
 * Read bluetooth address
 */
ReturnType Bluetooth_GetAddress(char *address)
{
	ReturnType ret;
	char temp[20];
	
	Bluetooth_SetMode(Mode_Command);
	
	ret = Bluetooth_SendCommand(bt_local_addr);
	
	// TODO: parse output
	
	address = temp;
	
	Bluetooth_SetMode(Mode_Server);
	
	return SUCCESS;
}

/**
 * Send command to bluetooth module
 */
ReturnType Bluetooth_SendCommand(const char *command)
{
	Uart_FlushRx();
	
	Uart_SendString((char *)command);
	Time_DelayMS(BT_WAIT_CMD_RESP);
	Uart_ReceiveString(cmd_resp_buffer, BT_CMD_RESP_BUF_SIZE);

#if BT_DEBUG_CMDMODE	
	strcat(resp, cmd_resp_buffer);
#endif

	if(strstr(cmd_resp_buffer, bt_success) != NULL)
	{
		return SUCCESS;
	}
	else
		return FAIL;
};
