#ifndef 	UKS_H
#define  	UKS_H

#include "stm32f4xx.h"

#define STEP_MOTOR_ROTATE_PWM	0x7F
#define STEP_MOTOR_ROTATE_PWM_DELAY	10

#define STEP_MOTOR_HOLDING_PWM	0x5F

#define STEP_MOTOR_PERIOD_MIN	10
#define STEP_MOTOR_PERIOD_MAX	1000

#define STEP_MOTOR_STARTING_PERIOD	200

enum
{
	TEST_STATE_STOP=0,
	TEST_STATE_GET_UP,
	TEST_STATE_GET_DOWN,
	TEST_STATE_CYCLE,
};

typedef struct
{
	uint8_t state;

} st_level_gage_test;

void Level_Gage_Test_Init(void);
void Level_Gage_Test_Restore_Settings(void);

#endif
