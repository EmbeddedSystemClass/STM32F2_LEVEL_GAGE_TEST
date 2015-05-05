#include "buzzer.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include <misc.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "watchdog.h"
#include "level_gage_test.h"

extern struct task_watch task_watches[];
xTaskHandle xBuzzer_Handle;

uint8_t buzzer_effect=BUZZER_EFFECT_NONE;

void Buzzer_Task(void *pvParameters );

void Buzzer_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure port -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);


    GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);

   xTaskCreate(Buzzer_Task,(signed char*)"BUZZER",64,NULL, tskIDLE_PRIORITY + 1, &xBuzzer_Handle);
}

void Buzzer_Task(void *pvParameters )
{
	while(1)
	{
		switch(buzzer_effect)
		{
			case BUZZER_EFFECT_0:
			{
				GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
				vTaskDelay(1500);
				GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
			}
			break;

			case BUZZER_EFFECT_1:
			{
				GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
				vTaskDelay(1000);
				GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
				vTaskDelay(1000);
				GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,1);
				vTaskDelay(1000);
				GPIO_WriteBit(BUZZER_PORT, BUZZER_PIN,0);
			}
			break;
		}

		task_watches[BUZZER_TASK].counter++;
		task_watches[BUZZER_TASK].task_status=TASK_IDLE;
		buzzer_effect=BUZZER_EFFECT_NONE;

		vTaskSuspend (xBuzzer_Handle);
	}
}


void Buzzer_Set_Buzz(uint8_t effect)
{
	vTaskResume(xBuzzer_Handle);
	task_watches[BUZZER_TASK].task_status=TASK_ACTIVE;
	buzzer_effect=effect&0x7;
}
