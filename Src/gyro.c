/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 8th, 2008
 * Content: Gyroscopic sensor routines 
 */
#include "segwii.h"
#include "gyro.h"
#include "adc.h"


/**
 * Initialize gyroscope
 */
void Gyro_Init(void)
{
}

/**
 * Sample rate from gyroscopic sensor
 */
void Gyro_SampleRate(GyroFilter *it, float in_rate, float interval)
{
	if(!it->rate_inited)
	{
		it->rate_bias = in_rate;
		it->rate_inited = 1;
	}
	
	it->rate = in_rate - it->rate_bias;
	it->angle += it->rate * interval;
	
	it->rate_bias += it->rate * 0.3 * interval;
}

/**
 * Sample angle from gyroscopic sensor
 */
void Gyro_SampleAngle(GyroFilter *it, float in_y, float interval)
{
	float angle_err;
	float in_angle = in_y * 2.0;
	
	if(!it->angle_inited)
	{
		it->angle = in_angle;
		it->angle_inited = 1;
	}
	
	angle_err = in_angle - it->angle;
	
	it->angle += angle_err * interval / (2.0 + it->angle_noise);
}

/**
 * Perform analysis on gyroscopic data
 */
void Gyro_PerformAnalysis(void)
{
	uint16_t tally;
	
	if(ADC_CollectSamples(SEGWII_ADC_ANGRATE_CH, &tally) == SUCCESS)
		printf_P(PSTR("t: %d\r\n"), tally - GYRO_MIDPOINT);
}
