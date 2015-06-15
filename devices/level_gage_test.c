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

#define CYCLE_LED_ON	GPIO_WriteBit(GPIOC, GPIO_Pin_6,1);
#define CYCLE_LED_OFF	GPIO_WriteBit(GPIOC, GPIO_Pin_6,0);

extern struct task_watch task_watches[];
extern xSemaphoreHandle xEndSwitchSemaphore;


void Level_Gage_Test_Task(void *pvParameters );

st_level_gage_test level_gage_test;


void Level_Gage_Test_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure port -------------------------------*/
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    CYCLE_LED_OFF;

	level_gage_test.test_state=TEST_STATE_STOP;
	level_gage_test.cycle_state=CYCLE_STATE_END;
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
					switch(level_gage_test.test_state)
					{
						case TEST_STATE_STOP:
						{
							 level_gage_test.cycle_state=CYCLE_STATE_SEARCH_END_SWITCH_LOWER;
							 level_gage_test.test_state=TEST_STATE_CYCLE;
							 Step_Motor_Set_Move_Type(MOVE_TYPE_CYCLE);
							 Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
							 CYCLE_LED_ON;
						}
						break;

						case TEST_STATE_GET_DOWN:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_STOP;
						}
						break;

						case TEST_STATE_GET_UP:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_STOP;
						}
						break;

						case TEST_STATE_CYCLE_PAUSE:
						{
							level_gage_test.test_state=TEST_STATE_CYCLE;
							 switch(level_gage_test.cycle_state)
							 {
									case CYCLE_STATE_SEARCH_END_SWITCH_LOWER:
									{
										Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
									}
									break;

									case CYCLE_STATE_GET_UP:
									{
										Step_Motor_Set_State(STEP_MOTOR_ROTATE_LEFT);
									}
									break;

									case CYCLE_STATE_GET_DOWN:
									{
										Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
									}
									break;

									case CYCLE_STATE_END:
									{
										level_gage_test.cycle_state=CYCLE_STATE_SEARCH_END_SWITCH_LOWER;
										Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
										CYCLE_LED_ON;
									}
									break;

									default:
									{

									}
							  }
							  Step_Motor_Set_Move_Type(MOVE_TYPE_CYCLE);
						}
						break;

						case TEST_STATE_CYCLE:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_CYCLE_PAUSE;
						}
						break;
					}
			   }
			   break;

			   case KBD_UP:
			   {
				   switch(level_gage_test.test_state)
					{
						case TEST_STATE_STOP:
						{
							level_gage_test.test_state=TEST_STATE_GET_UP;
							Step_Motor_Set_Move_Type(MOVE_TYPE_MANUAL);
							Step_Motor_Set_State(STEP_MOTOR_ROTATE_LEFT);
							level_gage_test.cycle_state=CYCLE_STATE_END;
							CYCLE_LED_OFF;
						}
						break;

						case TEST_STATE_GET_DOWN:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_STOP;
						}
						break;

						case TEST_STATE_GET_UP:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_STOP;
						}
						break;

						case TEST_STATE_CYCLE_PAUSE:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_STOP;
							level_gage_test.cycle_state=CYCLE_STATE_END;
							CYCLE_LED_OFF;
						}
						break;

						case TEST_STATE_CYCLE:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_CYCLE_PAUSE;
						}
						break;

						default:
						{

						}
						break;
					}
			   }
			   break;

			   case KBD_DOWN:
			   {
				   switch(level_gage_test.test_state)
					{
						case TEST_STATE_STOP:
						{
							level_gage_test.test_state=TEST_STATE_GET_DOWN;
							Step_Motor_Set_Move_Type(MOVE_TYPE_MANUAL);
							Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
							level_gage_test.cycle_state=CYCLE_STATE_END;
							CYCLE_LED_OFF;
						}
						break;

						case TEST_STATE_GET_DOWN:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_STOP;
						}
						break;

						case TEST_STATE_GET_UP:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_STOP;
						}
						break;

						case TEST_STATE_CYCLE_PAUSE:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_STOP;
							level_gage_test.cycle_state=CYCLE_STATE_END;
							CYCLE_LED_OFF;
						}
						break;

						case TEST_STATE_CYCLE:
						{
						    Step_Motor_Set_State(STEP_MOTOR_STOP);
						    level_gage_test.test_state=TEST_STATE_CYCLE_PAUSE;
						}
						break;

						default:
						{

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
	   if( xSemaphoreTake( xEndSwitchSemaphore, ( portTickType ) 1 ) == pdTRUE)
	   {
		switch(level_gage_test.test_state)
		{
			case TEST_STATE_STOP:
			{

			}
			break;

			case TEST_STATE_GET_DOWN:
			{
				if(step_motor.end_switch_state==END_SWITCH_LOWER)
				{
				    Step_Motor_Set_State(STEP_MOTOR_STOP);
				    level_gage_test.test_state=TEST_STATE_STOP;
				}
			}
			break;

			case TEST_STATE_GET_UP:
			{
				if(step_motor.end_switch_state==END_SWITCH_UPPER)
				{
				    Step_Motor_Set_State(STEP_MOTOR_STOP);
				    level_gage_test.test_state=TEST_STATE_STOP;
				}
			}
			break;

			case TEST_STATE_CYCLE_PAUSE:
			{

			}
			break;

			case TEST_STATE_CYCLE:
			{
				switch(level_gage_test.cycle_state)
				{
					case CYCLE_STATE_SEARCH_END_SWITCH_LOWER:
					{
						if(step_motor.end_switch_state==END_SWITCH_LOWER)
						{
							vTaskDelay(1000);
							level_gage_test.cycle_state=CYCLE_STATE_GET_UP;
							Step_Motor_Set_State(STEP_MOTOR_ROTATE_LEFT);
						}
					}
					break;

					case CYCLE_STATE_GET_UP:
					{
						 xSemaphoreTake( xADC_Mutex, portMAX_DELAY );
						 {
							  if(((int16_t)adc_channels.level_sensor_previous-(int16_t)adc_channels.level_sensor)>ADC_DELTA_ERROR)
							  {
								  Buzzer_Set_Buzz(BUZZER_EFFECT_2);

								  if(level_gage_test.hercon_error.error_position_counter<ERROR_POSITION_NUM)
								  {
									  level_gage_test.hercon_error.error_position[level_gage_test.hercon_error.error_position_counter]=adc_channels.level_sensor_previous;
									  level_gage_test.hercon_error.error_position_counter++;
								  }
							  }
						 }
						 xSemaphoreGive( xADC_Mutex );

						if(step_motor.end_switch_state==END_SWITCH_UPPER)
						{
							vTaskDelay(1000);
							level_gage_test.cycle_state=CYCLE_STATE_GET_DOWN;
							Step_Motor_Set_State(STEP_MOTOR_ROTATE_RIGHT);
						}
					}
					break;

					case CYCLE_STATE_GET_DOWN:
					{
						 xSemaphoreTake( xADC_Mutex, portMAX_DELAY );
						 {
							  if(((int16_t)adc_channels.level_sensor-(int16_t)adc_channels.level_sensor_previous)>ADC_DELTA_ERROR)
							  {
								  Buzzer_Set_Buzz(BUZZER_EFFECT_2);
							  }
						 }
						 xSemaphoreGive( xADC_Mutex );
						if(step_motor.end_switch_state==END_SWITCH_LOWER)
						{
							Step_Motor_Set_State(STEP_MOTOR_STOP);
							level_gage_test.cycle_state=CYCLE_STATE_END;
							level_gage_test.test_state=TEST_STATE_STOP;
							CYCLE_LED_OFF;
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
	  }
	  task_watches[DRYING_TASK].counter++;
	}
}

