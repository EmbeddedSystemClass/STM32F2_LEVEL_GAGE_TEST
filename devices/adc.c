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

xSemaphoreHandle xMeasure_Semaphore;

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

	   /* ��������� ������������ A��1 */
	   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	   /* ���������� ��������� ��� */
	   ADC_DeInit();
//
		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
		ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
		ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
		ADC_CommonInit(&ADC_CommonInitStructure);


	   ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	   ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	   ADC_InitStructure.ADC_NbrOfConversion=1;
	   ADC_Init(ADC1, &ADC_InitStructure);

	   ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_480Cycles);

	   ADC_Cmd(ADC1, ENABLE);
	   vSemaphoreCreateBinary( xMeasure_Semaphore );
	   xTaskCreate(ADC_Task,(signed char*)"ADC",128,NULL, tskIDLE_PRIORITY + 1, NULL);
}

#define NUM_CONV	16
#define MAX_ADC_CODE 	4095
#define MAX_ADC_VOLTAGE	3.3


static void ADC_Task(void *pvParameters)
{
		uint8_t i=0;
		uint32_t  sum_adc;
		task_watches[ADC_TASK].task_status=TASK_ACTIVE;

		while(1)
		{
			  sum_adc=0;
			  for(i=0;i<NUM_CONV;i++)
			  {
				   ADC_RegularChannelConfig(ADC1, ADC_CHN_SENSOR, 1, ADC_SampleTime_480Cycles);
				   ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;
				   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
				   {
					   taskYIELD ();
				   }
				   sum_adc+=ADC1->DR;
			  }

			  adc_channels.level_sensor=(uint16_t)(sum_adc/NUM_CONV);

			  sum_adc=0;
			  for(i=0;i<NUM_CONV;i++)
			  {
				   ADC_RegularChannelConfig(ADC1, ADC_CHN_VELOCITY_1, 1, ADC_SampleTime_480Cycles);
				   ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;
				   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
				   {
					   taskYIELD ();
				   }
				   sum_adc+=ADC1->DR;
			  }

			  adc_channels.velocity[0]=(uint16_t)(sum_adc/NUM_CONV);

			  sum_adc=0;
			  for(i=0;i<NUM_CONV;i++)
			  {
				   ADC_RegularChannelConfig(ADC1, ADC_CHN_VELOCITY_2, 1, ADC_SampleTime_480Cycles);
				   ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;
				   while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)
				   {
					   taskYIELD ();
				   }
				   sum_adc+=ADC1->DR;
			  }

			  adc_channels.velocity[1]=(uint16_t)(sum_adc/NUM_CONV);


			 // xSemaphoreGive(xMeasure_Semaphore);
			  vTaskDelay(100);
			  task_watches[ADC_TASK].counter++;
		}
}
