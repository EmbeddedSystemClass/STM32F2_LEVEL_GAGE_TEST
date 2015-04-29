#ifndef PWM_H
#define PWM_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include <misc.h>

extern uint8_t end_switch_state;

#define STEP_MOTOR_ROTATE_PWM	0x6F
#define STEP_MOTOR_ROTATE_PWM_DELAY	10

#define STEP_MOTOR_HOLDING_PWM	0x5F

#define STEP_MOTOR_PERIOD_MIN	10
#define STEP_MOTOR_PERIOD_MAX	1000

#define STEP_MOTOR_STARTING_PERIOD	200

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
void Step_Motor_Set_State(uint8_t state);

#endif
