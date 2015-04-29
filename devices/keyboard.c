#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_spi.h"

#include <misc.h>

#include "keyboard.h"
//Инклуды от FreeRTOS:



#include "buzzer.h"
#include "level_gage_test.h"

#include "watchdog.h"

extern st_level_gage_test level_gage_test;

static void vKeyboardTask(void *pvParameters);

extern struct task_watch task_watches[];

enum
{
	KEY_PRESSED=0,
	KEY_RELEASED,
};



uint8_t key_flags[3];

xQueueHandle xKeyQueue;

void Keyboard_Init(void)
{
	RCC_AHB1PeriphClockCmd(INIT_KEYB_PORT, ENABLE);
	GPIO_InitTypeDef init_pin;

	init_pin.GPIO_Pin  = KEY_0 | KEY_1 | KEY_2;
	init_pin.GPIO_Speed = GPIO_Speed_2MHz;
	init_pin.GPIO_Mode  = GPIO_Mode_IN;
	init_pin.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init (KEYB_PORT, &init_pin);

	key_flags[0]=KEY_RELEASED;
	key_flags[1]=KEY_RELEASED;
	key_flags[2]=KEY_RELEASED;

	xKeyQueue=xQueueCreate( 4, sizeof( uint8_t ) );
	xTaskCreate(vKeyboardTask,(signed char*)"Keyboard",64,NULL, tskIDLE_PRIORITY + 1, NULL);
}


static void vKeyboardTask(void *pvParameters)
{
	task_watches[KEYBOARD_TASK].task_status=TASK_ACTIVE;
	uint8_t key_code;
    while(1)
    {
    	if((GPIO_ReadInputDataBit(KEYB_PORT,KEY_0)==Bit_RESET)&&(key_flags[0]==KEY_RELEASED))
    	{
    		vTaskDelay(10);
    		if(GPIO_ReadInputDataBit(KEYB_PORT,KEY_0)==Bit_RESET)
    		{
    			key_code=KBD_0;
    			xQueueSend( xKeyQueue,&key_code, portMAX_DELAY );
    		}
    	}
    	else
    	{
    		key_flags[0]=KEY_RELEASED;
		}

    	if((GPIO_ReadInputDataBit(KEYB_PORT,KEY_1)==Bit_RESET)&&(key_flags[1]==KEY_RELEASED))
    	{
    		vTaskDelay(10);
    		if(GPIO_ReadInputDataBit(KEYB_PORT,KEY_1)==Bit_RESET)
    		{
    			key_code=KBD_1;
    			xQueueSend( xKeyQueue,&key_code, portMAX_DELAY );
    		}
    	}
    	else
    	{
    		key_flags[1]=KEY_RELEASED;
		}


    	if((GPIO_ReadInputDataBit(KEYB_PORT,KEY_2)==Bit_RESET)&&(key_flags[2]==KEY_RELEASED))
    	{
    		vTaskDelay(10);
    		if(GPIO_ReadInputDataBit(KEYB_PORT,KEY_2)==Bit_RESET)
    		{
    			key_code=KBD_2;
    			xQueueSend( xKeyQueue,&key_code, portMAX_DELAY );
    		}
    	}
    	else
    	{
    		key_flags[2]=KEY_RELEASED;
		}


    	vTaskDelay(50);
    	task_watches[KEYBOARD_TASK].counter++;
    }
}
