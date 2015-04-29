#include "level_gage_test.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdio.h"
#include "string.h"
#include "buzzer.h"
#include "watchdog.h"
#include "keyboard.h"
#include "adc.h"
#include "pwm.h"


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
	   if( uxQueueMessagesWaiting( xKeyQueue ) != 0 )
	   {
		   uint8_t key;
		   xQueueReceive( xKeyQueue, &key, 0 );

		   switch(key)
		   {
			   case KBD_CYCLE:
			   {

			   }
			   break;

			   case KBD_UP:
			   {
				   level_gage_test.state=TEST_STATE_GET_UP;
				   Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
			   }
			   break;

			   case KBD_DOWN:
			   {
				   level_gage_test.state=TEST_STATE_GET_DOWN;
				   Step_Motor_Set_State(STEP_MOTOR_ROTATE_LEFT);
			   }
			   break;

			   default:
			   {

			   }
			   break;
		   }
	   }
//--------------------------------------------------
	   switch(end_switch_state)
	   {
		   case END_SWITCH_UPPER:
		   {
			   level_gage_test.state=TEST_STATE_STOP;
		   }
		   break;

		   case END_SWITCH_LOWER:
		   {
			   level_gage_test.state=TEST_STATE_STOP;
		   }
		   break;

		   default:
		   {

		   }
		   break;
	   }
//--------------------------------------------------

//--------------------------------------------------
		switch(level_gage_test.state)
		{
			case TEST_STATE_STOP:
			{

			}
			break;

			case TEST_STATE_GET_DOWN:
			{
				uint16_t velocity;
				 xSemaphoreTake( xADC_Mutex, portMAX_DELAY );
				 {
					 velocity=adc_channels.velocity[1];
				 }
				 xSemaphoreGive( xADC_Mutex );

				 velocity/=10;

				 if(velocity<STEP_MOTOR_PERIOD_MIN)
				 {
					 velocity=STEP_MOTOR_PERIOD_MIN;
				 }

				 if(velocity>STEP_MOTOR_PERIOD_MAX)
				 {
					 velocity=STEP_MOTOR_PERIOD_MAX;
				 }
				Step_Motor_Set_Step_Period(velocity);
			}
			break;

			case TEST_STATE_GET_UP:
			{
				uint16_t velocity;
				 xSemaphoreTake( xADC_Mutex, portMAX_DELAY );
				 {
					 velocity=adc_channels.velocity[1];
				 }
				 xSemaphoreGive( xADC_Mutex );

				 velocity/=10;

				 if(velocity<STEP_MOTOR_PERIOD_MIN)
				 {
					 velocity=STEP_MOTOR_PERIOD_MIN;
				 }

				 if(velocity>STEP_MOTOR_PERIOD_MAX)
				 {
					 velocity=STEP_MOTOR_PERIOD_MAX;
				 }
				Step_Motor_Set_Step_Period(velocity);
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

