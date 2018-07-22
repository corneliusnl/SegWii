#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 7th, 2008
 * Content: bluetooth routines for BlueSMiRF module from SparkFun
 */
 
#define BT_DEBUG_CMDMODE		0	// allows debuging of command mode

#define BT_DEFAULT_SPEED		57600//9600
#define BT_HIGH_SPEED			57600

#define BT_CMD_RESP_BUF_SIZE	128
#define BT_WAIT_CMD_RESP		2500		// ms, to wait for response to command


#define BT_UART_PAR_NONE	0x0000
#define BT_UART_PAR_ODD		0x0001
#define BT_UART_PAR_EVEN	0x0002
#define BT_UART_PAR_MARK	0x0003
#define BT_UART_PAR_SPACE	0x0004

#define BT_UART_FLOW_NONE	0x0000
#define BT_UART_FLOW_RTSCTS	0x0001
#define BT_UART_FLOW_DTSDSR	0x0002

#define BT_CFG_PASSTHROUGH			0x01
#define BT_CFG_DTE_EMU				0x02
#define BT_CFG_IGNORE_ESC			0x04
#define BT_CFG_SUPPRESS_RESP		0x08
#define BT_CFG_DTRDSR_ENTER_CMD		0x10
#define BT_CFG_CONN_ACTIVE_GPIO		0x20


typedef enum
{
	Mode_Command,
	Mode_Client,
	Mode_Server,
} BluetoothMode;

void		Bluetooth_Init(void);
ReturnType	Bluetooth_SetSpeed(uint32_t speed);
ReturnType	Bluetooth_SetMode(BluetoothMode mode);
ReturnType	Bluetooth_SetName(const char *name);
ReturnType	Bluetooth_GetAddress(char *address);
ReturnType	Bluetooth_ResetToFactory(void);
ReturnType	Bluetooth_SaveToFlash(void);
ReturnType	Bluetooth_SetConfigFlag(uint8_t flag);


#endif
