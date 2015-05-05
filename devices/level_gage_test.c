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
	level_gage_test.test_state=TEST_STATE_STOP;
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
				   level_gage_test.test_state=TEST_STATE_CYCLE;

				   switch(step_motor.step_motor_state)
				   {
					   case STEP_MOTOR_ROTATE_LEFT:
					   {
						   Step_Motor_Set_State(STEP_MOTOR_STOP);
					   }
					   break;

					   case STEP_MOTOR_ROTATE_RIGHT:
					   {
						   Step_Motor_Set_State(STEP_MOTOR_STOP);
					   }
					   break;

					   case STEP_MOTOR_STOP:
					   {
						   level_gage_test.cycle_state=CYCLE_STATE_SEARCH_END_SWITCH_LOWER;
						   Step_Motor_Set_Move_Type(MOVE_TYPE_CYCLE);
					   }
					   break;
				   }
			   }
			   break;

			   case KBD_UP:
			   {
				   level_gage_test.test_state=TEST_STATE_GET_UP;

				   switch(step_motor.step_motor_state)
				   {
					   case STEP_MOTOR_ROTATE_LEFT:
					   {
						   Step_Motor_Set_State(STEP_MOTOR_STOP);
					   }
					   break;

					   case STEP_MOTOR_ROTATE_RIGHT:
					   {
						   Step_Motor_Set_State(STEP_MOTOR_STOP);
					   }
					   break;

					   case STEP_MOTOR_STOP:
					   {
						   Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
						   Step_Motor_Set_Move_Type(MOVE_TYPE_MANUAL);
					   }
					   break;
				   }
			   }
			   break;

			   case KBD_DOWN:
			   {
				   level_gage_test.test_state=TEST_STATE_GET_DOWN;

				   switch(step_motor.step_motor_state)
				   {
					   case STEP_MOTOR_ROTATE_LEFT:
					   {
						   Step_Motor_Set_State(STEP_MOTOR_STOP);
					   }
					   break;

					   case STEP_MOTOR_ROTATE_RIGHT:
					   {
						   Step_Motor_Set_State(STEP_MOTOR_STOP);
					   }
					   break;

					   case STEP_MOTOR_STOP:
					   {
						   Step_Motor_Set_State(STEP_MOTOR_ROTATE_LEFT);
						   Step_Motor_Set_Move_Type(MOVE_TYPE_MANUAL);
					   }
					   break;
				   }
			   }
			   break;

			   default:
			   {

			   }
			   break;
		   }
	   }
//--------------------------------------------------
//	   switch(step_motor.end_switch_state)
//	   {
//		   case END_SWITCH_UPPER:
//		   {
//			   level_gage_test.test_state=TEST_STATE_STOP;
//		   }
//		   break;
//
//		   case END_SWITCH_LOWER:
//		   {
//			   level_gage_test.test_state=TEST_STATE_STOP;
//		   }
//		   break;
//
//		   default:
//		   {
//
//		   }
//		   break;
//	   }
//--------------------------------------------------
		switch(level_gage_test.test_state)
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
				switch(level_gage_test.cycle_state)
				{
					case CYCLE_STATE_SEARCH_END_SWITCH_LOWER:
					{
						if(step_motor.end_switch_state!=END_SWITCH_LOWER)
						{
							Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
						}
						else
						{
							level_gage_test.cycle_state=CYCLE_STATE_GET_UP;
						}
					}
					break;

					case CYCLE_STATE_GET_UP:
					{
						if(step_motor.end_switch_state!=END_SWITCH_UPPER)
						{
							Step_Motor_Set_State(STEP_MOTOR_ROTATE_LEFT);
						}
						else
						{
							level_gage_test.cycle_state=CYCLE_STATE_GET_DOWN;
						}
					}
					break;

					case CYCLE_STATE_GET_DOWN:
					{
						if(step_motor.end_switch_state!=END_SWITCH_LOWER)
						{
							Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
						}
						else
						{
							Step_Motor_Set_State(STEP_MOTOR_STOP);
							level_gage_test.cycle_state=CYCLE_STATE_END;
						}
					}
					break;

					case CYCLE_STATE_END:
					{

					}
					break;

					default:
					{

					}
				}
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

