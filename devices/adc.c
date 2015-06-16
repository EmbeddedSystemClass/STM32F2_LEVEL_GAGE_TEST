#include "adc.h"
#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_adc.h>


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdio.h"
#include "string.h"

#include "level_gage_test.h"

#include "watchdog.h"

#define ADC_CHN_SENSOR		0
#define ADC_CHN_VELOCITY_1	11
#define ADC_CHN_VELOCITY_2	13

xSemaphoreHandle xADC_Mutex;

st_adc_channels adc_channels;

extern struct task_watch task_watches[];

static void ADC_Task(void *pvParameters);

void ADC_Channel_Init(void)
{
	   ADC_InitTypeDef ADC_InitStructure;
	   ADC_StructInit(&ADC_InitStructure);

	   ADC_CommonInitTypeDef ADC_CommonInitStructure;
	   ADC_CommonStructInit(&ADC_CommonInitStructure);

	   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	   GPIO_InitTypeDef gpio;
	   GPIO_StructInit(&gpio);
	   gpio.GPIO_Mode = GPIO_Mode_AN;
	   gpio.GPIO_Pin = GPIO_Pin_0;
	   GPIO_Init(GPIOA, &gpio);

	   GPIO_StructInit(&gpio);
	   gpio.GPIO_Mode = GPIO_Mode_AN;
	   gpio.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_3;
	   GPIO_Init(GPIOC, &gpio);

	   /* разрешаем тактирование AЦП1 */
	   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	   /* сбрасываем настройки АЦП */
	   ADC_DeInit();
//
		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
		ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
		ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
		ADC_CommonInit(&ADC_CommonInitStructure);


	   ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	   ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	   ADC_InitStructure.ADC_NbrOfConversion=1;
	   ADC_Init(ADC1, &ADC_InitStructure);

	   ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_480Cycles);

	   ADC_Cmd(ADC1, ENABLE);
	   xADC_Mutex=xSemaphoreCreateMutex();
	   xTaskCreate(ADC_Task,(signed char*)"ADC",128,NULL, tskIDLE_PRIORITY + 1, NULL);
}

#define NUM_CONV_SENSOR		32
#define NUM_CONV_RES		2
#define MAX_ADC_CODE 	4095
#define MAX_ADC_VOLTAGE	3.3


uint16_t abs(int16_t num)
{
	if(num>=0)
	{
		return (uint16_t)num;
	}
	else
	{
		return (uint16_t)(-num);
	}
}

#define CONTROL_DELTA	32

static void ADC_Task(void *pvParameters)
{
		uint8_t i=0;
		uint32_t  sum_adc_level_sensor, sum_adc_speed_manual_control, sum_adc_speed_cycle;
		task_watches[ADC_TASK].task_status=TASK_ACTIVE;
		//ADC_TempSensorVrefintCmd(ENABLE);
		while(1)
		{
			  sum_adc_level_sensor=0;
			  for(i=0;i<NUM_CONV_SENSOR;i++)
			  {
				   ADC_RegularChannelConfig(ADC1, ADC_CHN_SENSOR/*ADC_Channel_Vrefint*/, 1, ADC_SampleTime_480Cycles);
				   ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;
				   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
				   {
					   taskYIELD ();
				   }
				   sum_adc_level_sensor+=ADC1->DR;
			  }

			  sum_adc_speed_cycle=0;
			  for(i=0;i<NUM_CONV_RES;i++)
			  {
				   ADC_RegularChannelConfig(ADC1, ADC_CHN_VELOCITY_1, 1, ADC_SampleTime_480Cycles);
				   ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;
				   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
				   {
					   taskYIELD ();
				   }
				   sum_adc_speed_cycle+=ADC1->DR;
			  }

			  sum_adc_speed_manual_control=0;
			  for(i=0;i<NUM_CONV_RES;i++)
			  {
				   ADC_RegularChannelConfig(ADC1, ADC_CHN_VELOCITY_2, 1, ADC_SampleTime_480Cycles);
				   ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;
				   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
				   {
					   taskYIELD ();
				   }
				   sum_adc_speed_manual_control+=ADC1->DR;
			  }

			  if(abs((uint16_t)(sum_adc_speed_manual_control/NUM_CONV_RES)-adc_channels.speed_manual_control_previous)>CONTROL_DELTA)
			  {
				  adc_channels.speed_manual_control=(uint16_t)(sum_adc_speed_manual_control/NUM_CONV_RES) & 0xF00;
				  adc_channels.speed_manual_control_previous=(uint16_t)(sum_adc_speed_manual_control/NUM_CONV_RES);
			  }

			  if(abs((0xFFF-((uint16_t)(sum_adc_speed_cycle/NUM_CONV_RES)))-adc_channels.speed_cycle_previous)>CONTROL_DELTA)
			  {
				  adc_channels.speed_cycle=0xF00-((uint16_t)(sum_adc_speed_cycle/NUM_CONV_RES) & 0xF00);
				  adc_channels.speed_cycle_previous=(uint16_t)(sum_adc_speed_cycle/NUM_CONV_RES);
			  }


			 // adc_channels.speed_cycle=0xFFF-((uint16_t)(sum_adc_speed_cycle/NUM_CONV_RES)) ;

			 xSemaphoreTake( xADC_Mutex, portMAX_DELAY );
			 {
				  adc_channels.level_sensor_previous=adc_channels.level_sensor;
				  adc_channels.level_sensor=(uint16_t)(sum_adc_level_sensor/NUM_CONV_SENSOR);
//				  adc_channels.speed_cycle=/*0xF00*/0xFFF-((uint16_t)(sum_adc_speed_cycle/NUM_CONV_RES) /*& 0xF00*/);
//				  adc_channels.speed_manual_control=(uint16_t)(sum_adc_speed_manual_control/NUM_CONV_RES)/* & 0xF00*/;
			 }
			 xSemaphoreGive( xADC_Mutex );




			 vTaskDelay(100);
			 task_watches[ADC_TASK].counter++;
		}
}
