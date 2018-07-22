/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 9th, 2008
 * Content: Nintendo Wii Nunchuk i2c interfacing routines
 */
#include "segwii.h"
#include <math.h>
#include "nunchuk.h"
#include "i2c.h"
#include "uart.h"
#include "time.h"
#include "support.h"

/*
 * local variables
 */
 
// nunchuk sampling database
static volatile uint8_t nunchuk_data[NUNCHUK_SAMPLE_COUNT][6];
static volatile uint8_t nunchuk_data_idx = 0;

static NunchukCalibration nunchuk_cali;
static uint8_t init_nunchuk[]	= {0x40, 0x00};
static uint8_t read_sensors[]	= {0x00};
static uint8_t read_cali[]		= {0x20};//{0x17,0x04,0xA4,0x00,0x20,0x0,0x15};  


// local functions
void			Nunchuk_ReadCB(uint8_t *data, bool success);
void			Nunchuk_WriteCB(bool success);
NunchukData		*Nunchuk_ProcessPayload(uint8_t *data);

/**
 * Initialize communication with the Wii nunchuk
 */
void Nunchuk_Init(void)
{
	I2C_Init(I2C_MASTER, 400000);
	
	Nunchuk_ReadCalibration();
	
	// send command to nunchuk
	I2C_Start();
	I2C_WriteBytes(NUNCHUK_SLAVE_ADDR, init_nunchuk, 2);
	
	nunchuk_data_idx = 0;
}

/**
 * Read calibration
 */
void Nunchuk_ReadCalibration(void)
{
	uint8_t data[21] = {0x7D,0x7A,0x7E,0x0A,0xB0,0xAF,0xB1,0x12,0xE5,0x21,0x7A,0xE7,0x23,0x7B};
	uint8_t i;
	
	// request data
/*	I2C_Start();
	I2C_WriteBytes(NUNCHUK_SLAVE_ADDR, read_cali, 1);
	
	Time_DelayMS(2);
	// receive packet
	if(I2C_ReadBytes(NUNCHUK_SLAVE_ADDR, data, 16) == SUCCESS)*/
	if(1)
	{
		for(i = 0; i < 14; i++)
		{
//			printf_P(PSTR(" %02X "), data[i]);
			data[i] = NUNCHUK_DECODE_BYTE(data[i]);
		}
			
		// zero acceleration
		nunchuk_cali.zero_x			= (data[0] << 2) | ((data[3] & NUNCHUK_XCALI_MASK));
		nunchuk_cali.zero_y			= (data[1] << 2) | ((data[3] & NUNCHUK_YCALI_MASK) >> 2);
		nunchuk_cali.zero_z			= (data[2] << 2) | ((data[3] & NUNCHUK_ZCALI_MASK) >> 4);
		
	//	printf("D: %02X  ", data[3]);
		// gravity acceleration
		nunchuk_cali.gravity_x		= (data[4] << 2) | ((data[7] & NUNCHUK_XCALI_MASK));
		nunchuk_cali.gravity_y		= (data[5] << 2) | ((data[7] & NUNCHUK_YCALI_MASK) >> 2);
		nunchuk_cali.gravity_z		= (data[6] << 2) | ((data[7] & NUNCHUK_ZCALI_MASK) >> 4);
	//	printf("E: %02X  ", data[7]);
		// analog joystick x-axis calibration 
		nunchuk_cali.joy_x_max		= data[8];
		nunchuk_cali.joy_x_min		= data[9];
		nunchuk_cali.joy_x_center	= data[10];
		
		// analog joystick y-axis calibration
		nunchuk_cali.joy_y_max		= data[11];
		nunchuk_cali.joy_y_min		= data[12];
		nunchuk_cali.joy_y_center	= data[13];
	}	
}

/**
 * Perform analysis on sampled data
 */
void Nunchuk_PerformAnalysis(void)
{
	uint16_t tally[6];
	uint8_t i, j, count;

	cli();

	count = MIN(nunchuk_data_idx, NUNCHUK_SAMPLE_COUNT);
		
	printf_P(PSTR("cnt: %d "), count);
	for(i = 0; i < 6; i++)
	{
		for(j = 0; j < count; j++)
		{
			tally[i] += nunchuk_data[j][i]; 
		}
		
		tally[i] /= count;
	}
	
	Nunchuk_PrintData(Nunchuk_ProcessPayload(tally));
	
	nunchuk_data_idx = 0;
	
	sei();
}

/**
 * Start streaming
 */
void Nunchuk_StartStreaming()
{
	// Setup for callback mode
	I2C_SetWriteCallback(Nunchuk_WriteCB);
	I2C_SetReadCallback(Nunchuk_ReadCB);
	I2C_SetMode(I2C_CALLBACK);
	
	// start reading
	I2C_Start();
	I2C_WriteBytes(NUNCHUK_SLAVE_ADDR, read_sensors, 1);
}

/**
 * Callback for I2C write completion
 */
void Nunchuk_WriteCB(bool success)
{
	if(success)
		I2C_ReadBytes(NUNCHUK_SLAVE_ADDR, NULL, 6);
}

/**
 * Callback for I2C read operation
 */
void Nunchuk_ReadCB(uint8_t *data, bool success)
{
	uint8_t i;
	
	if(success)
	{
		for(i = 0; i < 6; i++)
			nunchuk_data[nunchuk_data_idx % NUNCHUK_SAMPLE_COUNT][i] = data[i];

		nunchuk_data_idx++;
	}
	
//	Time_DelayMS(650);
	

	// start next reading
	I2C_Start();
//	printf("  <C\r\n");
//	Uart_Process();
	I2C_WriteBytes(NUNCHUK_SLAVE_ADDR, read_sensors, 1);
}

/**
 * Receive data back from the nunchuck,
 */
NunchukData *Nunchuk_PollDevice(void)
{
	uint8_t i, mode;
	uint8_t nunchuk_payload[6];

	mode = I2C_GetMode();
	I2C_SetMode(I2C_BLOCKING);
	
	// request data
	I2C_Start();
	I2C_WriteBytes(NUNCHUK_SLAVE_ADDR, read_sensors, 1);
	
	Time_DelayMS(2);
	
	// receive packet
	if(I2C_ReadBytes(NUNCHUK_SLAVE_ADDR, nunchuk_payload, 6) == SUCCESS)
	{	
		I2C_SetMode(mode);
		return Nunchuk_ProcessPayload(nunchuk_payload);	
	}
	else
	{
		I2C_SetMode(mode);
		return NULL;
	}
}

/**
 * Get the delta between data reports
 */
ReturnType Nunchuk_GetDelta(NunchukData *result, NunchukData *data1, NunchukData *data2)
{
	if((data1 == NULL) || (data2 == NULL))
		return FAIL;
		
	result->joy_x		= data1->joy_x - data2->joy_x;
	result->joy_y		= data1->joy_y - data2->joy_y;
	result->accel_x		= data1->accel_x - data2->accel_x;
	result->accel_y		= data1->accel_y - data2->accel_y;
	result->accel_z		= data1->accel_z - data2->accel_z;
	result->buttons		= data2->buttons;

	return SUCCESS;
}

/**
 * Read Roll
 */
float Nunchuk_GetRoll(NunchukData *data)
{
	return asin(data->accel_x/nunchuk_cali.gravity_x);
}

/**
 * Read Pitch
 */
float Nunchuk_GetPitch(NunchukData *data)
{
	return asin(data->accel_y/nunchuk_cali.gravity_y);
}

/**
 * Process data payload from nunchuk
 */
NunchukData *Nunchuk_ProcessPayload(uint8_t *data)
{
	uint8_t i;
	static NunchukData tmp;
	
	for(i = 0; i < 6; i++){
//		printf_P(PSTR("%02X "), data[i]);
		data[i] = NUNCHUK_DECODE_BYTE(data[i]);
	}
			
	// perform post processing, decoding received data
	tmp.joy_x	= data[0] - nunchuk_cali.joy_x_center;
	tmp.joy_y	= data[1] - nunchuk_cali.joy_y_center;
	tmp.accel_x = ((data[2] << 2) | ((data[5] & NUNCHUK_XACCEL_MASK) >> 2)) - nunchuk_cali.zero_x;
	tmp.accel_y	= ((data[3] << 2) | ((data[5] & NUNCHUK_YACCEL_MASK) >> 4)) - nunchuk_cali.zero_y;
	tmp.accel_z	= ((data[4] << 2) | ((data[5] & NUNCHUK_ZACCEL_MASK) >> 6)) - nunchuk_cali.zero_z;
	tmp.buttons	= ~(data[5] & NUNCHUK_BUTTONS_MASK);
	
	return &tmp;
}

/**
 * print nunchuk data packet
 */
void Nunchuk_PrintData(NunchukData *data)
{
	if(data == NULL)
		return;
		
	Uart_Process();
	printf_P(PSTR("joy-x: %d\tjoy-y: %d\r\n"), data->joy_x, data->joy_y);
	printf_P(PSTR("accel-x: %d\taccel-y: %d\taccel-z: %d\r\n"), data->accel_x, data->accel_y, data->accel_z);
	printf_P(PSTR("button-c: %d\tbutton-z: %d\r\n"), (data->buttons & NUNCHUK_BUTTONC_MASK), (data->buttons & NUNCHUK_BUTTONZ_MASK) >> 1);
	Uart_Process();
}
