#ifndef __SEGWII_ADC_H__
#define __SEGWII_ADC_H__
/* 
 * SegWii
 * Copyright 2008 (c) Jeremiah Cornelius McCarthy.  All Rights Reserved.
 *
 * Date: March 8th, 2008
 */
 
#define ADC_HISTORY		20

typedef struct
{
	uint16_t sample[ADC_HISTORY];
	uint8_t index;
} ADC_SampleDB; 
 
#define ADC_CHANNEL_1	0x00
#define ADC_CHANNEL_2	0x01
#define ADC_CHANNEL_3	0x02
#define ADC_CHANNEL_4	0x03
#define ADC_CHANNEL_5	0x04
#define ADC_CHANNEL_6	0x05
#define ADC_CHANNEL_7	0x06
#define ADC_CHANNEL_8	0x07

//#ifndef ADC_CHANNELS_USED
#define ADC_CHANNELS_USED	3


#define ADC_MAX_VALUE	1023
#define ADC_VREF		5.0

#define ADC_SAMPLE_CONV (1.0 / 512.0 / (float)ADC_HISTORY)

#define ADC_CONVERT_TO_VOLT(a)	((a / (float)ADC_MAX_VALUE) * (float)ADC_VREF)

void		ADC_Init(void);
uint16_t	ADC_GetSingleSample(uint8_t channel);
void		ADC_StartStreaming(const uint8_t *sample_pattern, uint8_t count);
uint8_t		ADC_CollectSamples(uint8_t chan, uint16_t *sample_tally);
void		ADC_InterruptHandler(void);
uint8_t		ADC_MeasureBattery(void);

#endif

