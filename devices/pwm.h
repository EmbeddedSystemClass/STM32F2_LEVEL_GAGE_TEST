#ifndef PWM_H
#define PWM_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include <misc.h>

extern uint8_t end_switch_state;

enum
{
	END_SWITCH_NONE=0,
	END_SWITCH_UPPER,
	END_SWITCH_LOWER,
};

enum
{
	STEP_MOTOR_STOP=0,
	STEP_MOTOR_ROTATE_LEFT,
	STEP_MOTOR_ROTATE_RIGHT,
};

void PWM_Init(void);
void Step_Motor_Set_Step_Period(uint16_t period);

#endif
