/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: Feb 16th, 2008
 * Content: program entrypoint
 */
 
#include "includes.h"

void 		IOInit()
{
	// Setup data direction registers
	// 0 = input, 1 = output
	DDRA = 0xBC;					// oiooooii: 
	DDRB = 0xA0;					// oioiiiii: 
	DDRC = 0x09;					// iiiioiio: 
	DDRD = 0x00;					// iiiiiiii:
	DDRE = 0x22;					// iioiiioi: 
	DDRF = 0x00;					// iiiiiiii: 
	PORTF = 0x00;					// pull-up off
	DDRG = 0x00;
}


BOOLEAN 	IODebounce(INT8U port, INT8U pin)
{
	INT8U count 	= 0;
	INT8U i			= 0;
	
	for(i = 0; i < 3; i++)
	{
		if( IOGetPin(port, pin) )
		{
			count++;
		}
	}
	
	return (count > 1);
}

