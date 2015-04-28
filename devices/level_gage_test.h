#ifndef 	UKS_H
#define  	UKS_H

#include "stm32f4xx.h"

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
