#include "level_gage_test.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdio.h"
#include "string.h"
#include "buzzer.h"
#include "watchdog.h"


extern struct task_watch task_watches[];

void Level_Gage_Test_Task(void *pvParameters );

st_level_gage_test level_gage_test;


void Level_Gage_Test_Init(void)
{
	level_gage_test.state=TEST_STATE_STOP;
	xTaskCreate(Level_Gage_Test_Task,(signed char*)"LEVEL GAGE TEST TASK",128,NULL, tskIDLE_PRIORITY + 1, NULL);
}


void Level_Gage_Test_Task(void *pvParameters )
{
	task_watches[DRYING_TASK].task_status=TASK_ACTIVE;
	while(1)
	{
		switch(level_gage_test.state)
		{
			case TEST_STATE_STOP:
			{

			}
			break;

			case TEST_STATE_GET_DOWN:
			{

			}
			break;

			case TEST_STATE_GET_UP:
			{

			}
			break;

			case TEST_STATE_CYCLE:
			{

			}
			break;

			default:
			{

			}
			break;
		}
		task_watches[DRYING_TASK].counter++;
	}
}

