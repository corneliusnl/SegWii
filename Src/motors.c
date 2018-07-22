/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Feb 16th, 2008
 * Content: motor control for dual Open Source Motor Controllers (OSMC)
 *
 * PORTC.2 - Right disable (active high)
 * PORTC.3 - Left disable (active high)
 * PORTC.4 - Right direction
 * PORTC.5 - Left direction 
 */
#include "segwii.h"
#include "timers.h"
#include "motors.h"
#include "support.h"

// Encoder variables
static volatile uint16_t L_current_state;
static volatile uint16_t R_current_state;
static volatile uint16_t L_last_state;
static volatile uint16_t R_last_state;
static volatile uint16_t current_position_L;
static volatile uint16_t current_position_R;

/**
 * Initialize motors
 */
void Motors_Init(void)
{
	// Configure OSMC pins
	DDRC	= DDRC | 0x3C;
	
	// Setup Initial Conditions
	Motors_Enable(MtrNone);
	Motors_Brake();
	Motors_Setup(DirForward);
	
	// configure Timer1 PWM
	Timer1_SetupPWM();
	
	// enabler external interrupts for encoders (falling edge triggered)
	MCUCR	= (1 << ISC11) | (1 << ISC01);
	GICR	= (1 << INT1) | (1 << INT0);
}

/*
 * Setup motor directions
 *
 * dir - FORWARD, REVERSE, LEFT, RIGHT
 */
ReturnType Motors_Setup(MotorDir dir)
{
	switch(dir)
	{
	case DirForward:
		IOSetPinLo(PORTC, MOTOR_LEFT_DIR_PIN);
		IOSetPinLo(PORTC, MOTOR_RIGHT_DIR_PIN);
		break;
	case DirReverse:
		IOSetPinHi(PORTC, MOTOR_LEFT_DIR_PIN);
		IOSetPinHi(PORTC, MOTOR_RIGHT_DIR_PIN);
		break;
	case DirRight:
		IOSetPinLo(PORTC, MOTOR_LEFT_DIR_PIN);	
		IOSetPinHi(PORTC, MOTOR_RIGHT_DIR_PIN);
		break;
	case DirLeft:
		IOSetPinHi(PORTC, MOTOR_LEFT_DIR_PIN);
		IOSetPinLo(PORTC, MOTOR_RIGHT_DIR_PIN);
		break;
	default:
		return FAIL;
	}
	
	return SUCCESS;
}
 
/*
 * Move robot straight by synchronizing with encoder values
 *
 * dir		- FORWARD, REVERSE
 * distance	- distance to travel in centimeters. value of 0 will drive until obstacle.
 */
ReturnType Motors_DriveStraight(MotorDir dir, uint16_t distance)
{
	long duty_L, duty_R, motor_speed;
	float adj;

	// only forward or reverse
	if((dir != DirForward) && (dir != DirReverse))
		return FAIL;
		
	Motors_Setup(dir);
	Motors_ResetCounts();
	
	motor_speed = MOTOR_FULL_SPEED;

	while(1)
	{			
		// PID control -- synchronize motors
		adj = (float)((float)MOTOR_PID_GAIN * (float)(current_position_R - (float)current_position_L));
	
		if(adj != 0)
		{
			duty_L = motor_speed + (long)adj;
			duty_R = motor_speed - (long)adj;
		}
	
		// update motor speeds
		Motors_SetDutyL(duty_L);
		Motors_SetDutyR(duty_R);
	}
	
	Motors_Brake();
	
	return SUCCESS;
}

/*
 * Turn robot
 *
 * angle	- value of degrees to turn. 0-180*
 */
void Motors_Turn(MotorDir dir, uint8_t angle)
{
	uint16_t clk_count;
	float adj;
	long duty_L, duty_R;
	
	Motors_ResetCounts();

	Motors_Setup(dir);
		
	clk_count = (uint16_t)(angle*(float)MOTOR_ENC_CLK_PER_DEG);

	while(current_position_R < clk_count)
	{
		// PID control -- synchronize motors
		adj = (float)((float)MOTOR_TURN_PID_GAIN * (float)(current_position_R - (float)current_position_L));
		if(adj != 0.0)
		{
			duty_L	= MOTOR_TURN_SPEED + (long)adj;
			duty_R	= MOTOR_TURN_SPEED - (long)adj;
		}
		
		Motors_SetDutyL(duty_L);
		Motors_SetDutyR(duty_R);
	}
	
	Motors_Brake();
}


/*
 * Robot brake
 */
void Motors_Brake(void)
{
	Motors_SetDutyR(0);
	Motors_SetDutyL(0);	
}

/**
 * Enable/disable motor(s)
 */
void Motors_Enable(Motor motor)
{
	switch(motor)
	{
	case MtrLeft:
		IOSetPin(PORTC, MOTOR_LEFT_DIS_PIN, true); 
		IOSetPin(PORTC, MOTOR_RIGHT_DIS_PIN, false);
		break;
	case MtrRight:
		IOSetPin(PORTC, MOTOR_LEFT_DIS_PIN, false);
		IOSetPin(PORTC, MOTOR_RIGHT_DIS_PIN, true);
		break;
	case MtrNone:
		IOSetPin(PORTC, MOTOR_LEFT_DIS_PIN, false);
		IOSetPin(PORTC, MOTOR_RIGHT_DIS_PIN, false);	
		break;
	case MtrBoth:
		IOSetPin(PORTC, MOTOR_LEFT_DIS_PIN, true);
		IOSetPin(PORTC, MOTOR_RIGHT_DIS_PIN, true);
		break;
	}
}

/*
 * Reset motor counts
 */
void Motors_ResetCounts(void)
{
	current_position_R = 0;
	current_position_L = 0;
}

/**
 * Interrupt handler
 */
inline void Motors_InterruptHandler(void)
{
	// right encoder
	R_last_state = R_current_state;
	
	R_current_state = (PORTC & ENCODER_RIGHT_PIN);

	if(R_current_state != R_last_state)
		current_position_R++;

	// left encoder
	L_last_state = L_current_state;			
	
	L_current_state = (PORTC & ENCODER_LEFT_PIN);

	if(L_current_state != L_last_state)
		current_position_L++;
}
