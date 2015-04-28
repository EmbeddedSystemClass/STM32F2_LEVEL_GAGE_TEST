#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define KEYB_PORT    	GPIOB
#define INIT_KEYB_PORT 	RCC_AHB1Periph_GPIOB


#define KEY_0 GPIO_Pin_13
#define KEY_1 GPIO_Pin_14
#define KEY_2 GPIO_Pin_15

extern xQueueHandle xKeyQueue;
void Keyboard_Init(void);


#endif
