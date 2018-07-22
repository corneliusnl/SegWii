#ifndef __SEGWII_MOTORS_H__
#define __SEGWII_MOTORS_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 */
 
/*
 * Defines
 */
#define MOTOR_ENC_CLK_PER_REV		1920
#define MOTOR_ENC_CLK_PER_90		1050 // 1200
// perform calculations to save processing power
//#define MOTOR_ENC_CLK_PER_DEG		11.0
#define MOTOR_ENC_CLK_PER_DEG	(float)(MOTOR_ENC_CLK_PER_90 / 90.0)
//#define MOTOR_ENC_CLK_PER_HALFDEG	6
#define MOTOR_ENC_CLK_PER_HALFDEG	(float)(MOTOR_ENC_CLK_PER_90 / 180.0)
#define MOTOR_DIST_CM_PER_CLK		0.025 				// WHEEL_CIRCUMFERENCE_CM / MOTOR_ENC_CLK_PER_REV
#define MOTOR_ENC_CLK_PER_CM		40

#define MOTOR_LEFT_DIR_PIN		5
#define MOTOR_RIGHT_DIR_PIN		4
#define MOTOR_LEFT_DIS_PIN		3
#define MOTOR_RIGHT_DIS_PIN		2

#define MOTOR_TURN_SPEED		120
#define MOTOR_ALIGN_SPEED		35
#define MOTOR_FULL_SPEED		300
#define MOTOR_SPEED_DIFF		50
#define MOTOR_REVERSE_SPEED		170

#define MOTOR_RAMP_UP_CLKS		1500
#define MOTOR_RAMP_SLOPE		(float)(MOTOR_SPEED_DIFF/(float)MOTOR_RAMP_UP_CLKS)
#define MOTOR_PID_GAIN			0.15
#define MOTOR_TURN_PID_GAIN		0.7


typedef enum
{
	DirForward,
	DirReverse,
	DirLeft, 
	DirRight
} MotorDir;

typedef enum
{
	MtrLeft,
	MtrRight,
	MtrBoth,
	MtrNone,
} Motor;


#define Motors_SetDutyR(a)		Timer1_SetDutyA(a)
#define Motors_SetDutyL(a)		Timer1_SetDutyB(a)

void		Motors_Init(void);
ReturnType	Motors_DriveStraight(MotorDir dir, uint16_t distance);
ReturnType	Motors_Setup(MotorDir dir);
void		Motors_Turn(MotorDir dir, uint8_t angle);
void		Motors_Brake(void);
void		Motors_ResetCounts(void);
void		Motors_Enable(Motor motor);
inline void	Motors_InterruptHandler(void);


#endif
