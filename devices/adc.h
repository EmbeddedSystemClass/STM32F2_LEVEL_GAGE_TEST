#ifndef ADC_H
#define ADC_H

#include "stm32f4xx.h"
#include "level_gage_test.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define ADC_LM35_CHANNELS_NUM 	DRYING_CHANNELS_NUM
#define ADC_FILTER_BUFFER_LEN	16



#define ADC_DELTA_ERROR			0x7FF

extern xSemaphoreHandle xADC_Mutex;


typedef struct
{
	uint16_t speed_manual_control;
	uint16_t speed_cycle;
	uint16_t level_sensor;
	uint16_t level_sensor_previous;

} st_adc_channels;

extern st_adc_channels adc_channels;

void ADC_Channel_Init(void);

#endif
