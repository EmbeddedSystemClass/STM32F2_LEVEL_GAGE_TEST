#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f4xx.h"

enum
{
	BUZZER_EFFECT_0=0,
	BUZZER_EFFECT_1,
	BUZZER_EFFECT_2,
	BUZZER_EFFECT_NONE,
};

#define BUZZER_PIN	GPIO_Pin_7
#define BUZZER_PORT GPIOC


void Buzzer_Init(void);
void Buzzer_Set_Buzz(uint8_t effect);


#endif
