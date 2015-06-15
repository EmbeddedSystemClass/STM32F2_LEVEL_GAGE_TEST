#ifndef PWM_H
#define PWM_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include <misc.h>

//#include "semphr.h"

extern uint8_t end_switch_state;

#define STEP_MOTOR_ROTATE_PWM	0x9F
#define STEP_MOTOR_ROTATE_PWM_DELAY	10

#define STEP_MOTOR_HOLDING_PWM	0x5F

#define STEP_MOTOR_PERIOD_MIN	10
#define STEP_MOTOR_PERIOD_MAX	1000

#define STEP_MOTOR_STARTING_PERIOD	200

typedef enum
{
	END_SWITCH_NONE=0,
	END_SWITCH_UPPER,
	END_SWITCH_LOWER,
} en_end_switch_state;

typedef enum
{
	STEP_MOTOR_STOP=0,
	STEP_MOTOR_ROTATE_LEFT,
	STEP_MOTOR_ROTATE_RIGHT,
} en_step_motor_state;

typedef enum
{
	MOVE_TYPE_MANUAL=0,
	MOVE_TYPE_CYCLE,

} en_step_motor_move_type;

typedef struct
{
	uint16_t step_period;
	uint16_t step_starting_counter;
	en_step_motor_state step_motor_state;
	en_end_switch_state end_switch_state;
	en_step_motor_move_type step_motor_move_type;
	uint8_t cycle_counter;
}st_step_motor;

extern st_step_motor step_motor;


void Step_Motor_Init(void);
void Step_Motor_Set_Step_Period(uint16_t period);
void Step_Motor_Set_State(uint8_t state);
void Step_Motor_Set_Move_Type( en_step_motor_move_type move_type);

#endif
