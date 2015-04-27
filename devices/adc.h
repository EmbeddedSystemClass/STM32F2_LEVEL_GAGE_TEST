#ifndef ADC_H
#define ADC_H

#include "stm32f4xx.h"
#include "level_gage_test.h"

#define ADC_LM35_CHANNELS_NUM 	DRYING_CHANNELS_NUM
#define ADC_FILTER_BUFFER_LEN	16

typedef struct
{
	uint16_t velocity[2];
	uint16_t level_sensor;
} st_adc_channels;

void ADC_Channel_Init(void);

#endif
