#ifndef __SEGWII_NUNCHUK_H__
#define __SEGWII_NUNCHUK_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 9th, 2008
 * Content: Nintendo Wii Nunchuk i2c interfacing routines
 */

typedef struct
{
	// thumb joystick
	int8_t joy_x;
	int8_t joy_y;
	
	// accelerometer data (10-bit)
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	
	// button data
	uint8_t buttons;
} NunchukData;

typedef struct
{
	// accelerometer zero values
	uint16_t	zero_x;
	uint16_t	zero_y;
	uint16_t	zero_z;
	// earth gravitation
	uint16_t	gravity_x;
	uint16_t	gravity_y;
	uint16_t	gravity_z; 
	// joystict x-axis
	uint8_t		joy_x_min;
	uint8_t		joy_x_center;
	uint8_t		joy_x_max;
	// Joystick y-axis
	uint8_t		joy_y_min;
	uint8_t		joy_y_center; 
	uint8_t		joy_y_max; 
} NunchukCalibration;

#define NUNCHUK_SLAVE_ADDR 		0x52

#define NUNCHUK_SAMPLE_COUNT	20

#define NUNCHUK_DECODE_BYTE(a)	((a ^ 0x17) + 0x17)

#define NUNCHUK_BUTTONS_MASK	0x03
#define NUNCHUK_BUTTONC_MASK	0x02
#define NUNCHUK_BUTTONZ_MASK	0x01
#define NUNCHUK_XACCEL_MASK		0x0C
#define NUNCHUK_YACCEL_MASK		0x30
#define NUNCHUK_ZACCEL_MASK		0xC0
#define NUNCHUK_XCALI_MASK		0x03
#define NUNCHUK_YCALI_MASK		0x0C
#define NUNCHUK_ZCALI_MASK		0x30

void		Nunchuk_Init(void);
void		Nunchuk_PrintData(NunchukData *data);
void		Nunchuk_StartStreaming(void);
NunchukData *Nunchuk_PollDevice(void);
void		Nunchuk_PerformAnalysis(void);
void		Nunchuk_ReadCalibration(void);
ReturnType	Nunchuk_GetDelta(NunchukData *result, NunchukData *data1, NunchukData *data2);
float		Nunchuk_GetRoll(NunchukData *data);
float		Nunchuk_GetPitch(NunchukData *data);

#endif
