#ifndef 	UKS_H
#define  	UKS_H

#include "stm32f4xx.h"

enum
{
	DRYING_CONTINUE=0,
	DRYING_DONE,
	DRYING_WAIT_NEW_OPERATION
};

struct parameters
{

};

struct level_gage_test
{

};

void Level_Gage_Test_Init(void);
void Level_Gage_Test_Restore_Settings(void);

#endif
