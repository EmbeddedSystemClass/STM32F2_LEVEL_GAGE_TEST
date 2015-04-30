#ifndef 	UKS_H
#define  	UKS_H

#include "stm32f4xx.h"



typedef enum
{
	TEST_STATE_STOP=0,
	TEST_STATE_GET_UP,
	TEST_STATE_GET_DOWN,
	TEST_STATE_CYCLE,
} en_test_state;

typedef enum
{
	CYCLE_STATE_SEARCH_END_SWITCH_LOWER=0,
	CYCLE_STATE_GET_UP,
	CYCLE_STATE_GET_DOWN,
	CYCLE_STATE_END,

} en_cycle_state;

typedef struct
{
	en_test_state test_state;
	en_cycle_state cycle_state;


} st_level_gage_test;

void Level_Gage_Test_Init(void);
void Level_Gage_Test_Restore_Settings(void);

#endif
