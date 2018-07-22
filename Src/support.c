/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 7th, 2008
 * Content: supporting routines
 */
#include "segwii.h"
#include "support.h"
 
/**
 * low pass float update
 */
void lpf_update(float *state, float tc, float interval, float input)
{
	float frac = interval / tc;
	if(frac > 1.0)
		frac = 1.0;
	*state = input * frac + *state * (1.0 - frac);
}

/**
 * float with limits
 */
float flim(float x, float lo, float hi)
{
	if(x > hi) return hi;
	if(x < lo) return lo;
	return x;
}
