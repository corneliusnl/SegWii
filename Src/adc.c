/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 8th, 2008
 * Content: Analog to digital conversion routines
 */
#include "segwii.h"
#include "adc.h"
#include "uart.h"
#include "support.h"

static ADC_SampleDB	adc_sample_db[8];

// sampling pattern
static uint8_t	*adc_pattern		= NULL;
static uint8_t	adc_pattern_count	= 0;
static uint8_t	adc_pattern_idx		= 0;

/**
 * Initialize the ADC
 */
void ADC_Init(void)
{
	// Input, no pullups
	PORTA	= 0x00;
	DDRA	= 0x00;
	
	// Turn off comparator
	ACSR |= (1 << ACD);
	
	// enable ADC - divide by 128, enable interrupt, start conversion
	ADCSRA = (0x07 << ADPS0) | (1 << ADEN);
}

/**
 * Take a single sample
 */
uint16_t ADC_GetSingleSample(uint8_t channel)
{
	uint8_t adcsra;
	uint16_t temp;
	
	ADMUX = 0x40 | channel;
	
	adcsra = ADCSRA;
	
	// enable and start conversion
	// TODO: mask appropriate bits OR with ADSC
	ADCSRA = (0x07 << ADPS0) | (1 << ADEN) | (1 << ADSC);
	loop_until_bit_is_set(ADCSRA, ADIF);
	
	temp = ADC;
	
	ADCSRA = adcsra;
	
	return temp;
	
}

/** 
 * Start repetitive sampling
 */
void ADC_StartStreaming(const uint8_t *sample_pattern, uint8_t count)
{
	uint8_t	i;
	
	if(count == 0)
		return;

	// setup sampling pattern
	adc_pattern			= sample_pattern;
	adc_pattern_count	= count;
	adc_pattern_idx		= 0;
	
	// setup database of samples
	for(i = 0; i < 8; i++)
	{
		adc_sample_db[i].index = 0;
	}
		
	// config ADC and enable interrupts to signal completion
	ADMUX	= 0x40 | adc_pattern[0];
	ADCSRA	= (0x07 << ADPS0) | (1 << ADEN) | (1 << ADIE) | (1 << ADSC);
}

/**
 * Collect samples
 */
ReturnType ADC_CollectSamples(uint8_t chan, uint16_t *sample_tally)
{
	ADC_SampleDB *db;
	uint8_t i, count;
	
	cli();
	
	db = &adc_sample_db[chan];
	if(db->index == 0)
	{
		sei();
		return FAIL;
	}

	count = MIN(db->index, ADC_HISTORY);
	*sample_tally = 0;
	for (i = 0; i < count; i++)
		*sample_tally += db->sample[i];
	db->index = 0;
	
	printf_P(PSTR("ADC_CS cnt: %d tal: %d\r\n"), count, *sample_tally);
	Uart_Process();
	*sample_tally /= count;
	
	sei();
	
	return SUCCESS;
}

/**
 * ADC interrupt handler
 */
void ADC_InterruptHandler(void)
{
	ADC_SampleDB *db;
	uint8_t adc_chan;
	
	// get channel that was measured
	adc_chan = ADMUX & 0x07;
	
	// store value
	db = &adc_sample_db[adc_chan];
	db->sample[db->index % ADC_HISTORY] = ADCW;
	db->index++;
	
	// get next channel from pattern
	adc_pattern_idx++;
	if (adc_pattern_idx >= adc_pattern_count)
		adc_pattern_idx = 0;
	
	// set next channel for conversion	
	ADMUX = 0x40 | adc_pattern[adc_pattern_idx];
	
	// trigger next conversion
	ADCSRA |= (1 << ADSC);
}

/**
 * Measure battery
 */
uint8_t ADC_MeasureBattery(void)
{
/*	float batt_volatage;
	uint16_t samples;
	
	if(!ADC_CollectSamples(ADC_BATTERY_CH, &samples, &last_balance_s0))
		return;
		
	batt_voltage = samples * ADC_SAMPLE_CONV * 0.5;*/
}
