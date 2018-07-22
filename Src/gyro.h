#ifndef __SEGWII_GYRO_H__
#define __SEGWII_GYRO_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 8th, 2008
 */

/*
  A basic 2-input Kalman filter for a rate gyro stabilized with a 2-axis accelerometer.
  It's really wrong to use the angle; I should handle sin and cosine terms separately.
 */
typedef struct 
{
	float angle;
	float rate_bias;
	float rate;
	float angle_noise;
	
	uint8_t rate_inited;
	uint8_t angle_inited;
} GyroFilter;

#define GYRO_MIDPOINT	490

void Gyro_Init(void);
void Gyro_SampleRate(GyroFilter *it, float in_rate, float interval);
void Gyro_SampleAngle(GyroFilter *it, float in_y, float interval);
void Gyro_PerformAnalysis(void);

#endif
