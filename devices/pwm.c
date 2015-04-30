#include "pwm.h"
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "level_gage_test.h"
#include "adc.h"

#define PHAZE_0   TIM3->CCR1
#define PHAZE_1   TIM3->CCR2
#define PHAZE_2   TIM4->CCR1
#define PHAZE_3   TIM4->CCR2

#define END_SWITCH_PORT			GPIOA
#define END_SWITCH_LOWER_PIN	GPIO_Pin_6
#define END_SWITCH_UPPER_PIN	GPIO_Pin_7

st_step_motor step_motor;

void Step_Motor_Task(void *pvParameters );
void Step_Motor_Step(void);
void Step_Motor_Step_Left(void);
void Step_Motor_Step_Right(void);
void Step_Motor_Hold(void);

void Step_Motor_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseStructInit( &TIM_TimeBaseInitStruct );
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV4;
    TIM_TimeBaseInitStruct.TIM_Period = 256;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 60;

    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseInitStruct );
    TIM_TimeBaseInit( TIM4, &TIM_TimeBaseInitStruct );

    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCStructInit( &TIM_OCInitStruct );
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;

    TIM_OCInitStruct.TIM_Pulse = 0;

    TIM_OC1Init( TIM3, &TIM_OCInitStruct );
    TIM_OC2Init( TIM3, &TIM_OCInitStruct );

    TIM_OC1Init( TIM4, &TIM_OCInitStruct );
    TIM_OC2Init( TIM4, &TIM_OCInitStruct );

    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE);

    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);

    PHAZE_0=0;
    PHAZE_1=0;
    PHAZE_2=0;
    PHAZE_3=0;

    step_motor.step_motor_state=STEP_MOTOR_STOP;
    step_motor.step_period=100;
    step_motor.cycle_counter=0;
    step_motor.step_starting_counter=STEP_MOTOR_STARTING_PERIOD;
    step_motor.end_switch_state=END_SWITCH_NONE;
    step_motor.step_motor_move_type=MOVE_TYPE_MANUAL;

    GPIO_InitStructure.GPIO_Pin =END_SWITCH_LOWER_PIN | END_SWITCH_UPPER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(END_SWITCH_PORT, &GPIO_InitStructure);

    xTaskCreate(Step_Motor_Task,(signed char*)"STEP MOTOR TASK",64,NULL, tskIDLE_PRIORITY + 1, NULL);
}

void Step_Motor_Task(void *pvParameters )
{
	while(1)
	{
		 xSemaphoreTake( xADC_Mutex, portMAX_DELAY );
		 {

			 if(step_motor.step_motor_move_type==MOVE_TYPE_MANUAL)
			 {
				 step_motor.step_period=adc_channels.speed_manual_control;
			 }
			 else if(step_motor.step_motor_move_type==MOVE_TYPE_CYCLE)
			 {
				 step_motor.step_period=adc_channels.speed_cycle;
			 }
		 }
		 xSemaphoreGive( xADC_Mutex );

		 step_motor.step_period/=10;

		 if(step_motor.step_period<STEP_MOTOR_PERIOD_MIN)
		 {
			 step_motor.step_period=STEP_MOTOR_PERIOD_MIN;
		 }

		 if(step_motor.step_period>STEP_MOTOR_PERIOD_MAX)
		 {
			 step_motor.step_period=STEP_MOTOR_PERIOD_MAX;
		 }
//---------------------------------------------------

		if((step_motor.step_starting_counter>step_motor.step_period)&&(step_motor.step_motor_state!=STEP_MOTOR_STOP))
		{
			step_motor.step_starting_counter-=50;
			vTaskDelay(step_motor.step_starting_counter);
		}
		else
		{
			vTaskDelay(step_motor.step_period);
		}
//------------------------------------------------
		if(GPIO_ReadInputDataBit(END_SWITCH_PORT,END_SWITCH_LOWER_PIN)==Bit_RESET)
		{
			step_motor.end_switch_state=END_SWITCH_LOWER;
		}
		else
		{
			if(GPIO_ReadInputDataBit(END_SWITCH_PORT,END_SWITCH_UPPER_PIN)==Bit_RESET)
			{
				step_motor.end_switch_state=END_SWITCH_UPPER;
			}
			else
			{
				step_motor.end_switch_state=END_SWITCH_NONE;
			}
		}
//-----------------------------------------------------------
		switch(step_motor.step_motor_state)
		{
			case  STEP_MOTOR_ROTATE_LEFT:
			{
				if(step_motor.end_switch_state==END_SWITCH_UPPER)
				{
					step_motor.step_motor_state=STEP_MOTOR_STOP;
				}
				else
				{
					Step_Motor_Step_Left();
				}
			}
			break;

			case  STEP_MOTOR_ROTATE_RIGHT:
			{
				if(step_motor.end_switch_state==END_SWITCH_LOWER)
				{
					step_motor.step_motor_state=STEP_MOTOR_STOP;
				}
				else
				{
					Step_Motor_Step_Right();
				}
			}
			break;

			case  STEP_MOTOR_STOP:
			{
				Step_Motor_Hold();
				step_motor.step_starting_counter=STEP_MOTOR_STARTING_PERIOD;
			}
			break;

			default:
			{

			}
			break;
		}
	}
}

void Step_Motor_Set_State(uint8_t state)
{
	step_motor.step_motor_state=state;
}

uint8_t Step_Motor_Get_State(void)
{
	return step_motor.step_motor_state;
}


void Step_Motor_Set_Step_Period(uint16_t period)
{
	step_motor.step_period=period;
}

void Step_Motor_Set_Move_Type( en_step_motor_move_type move_type)
{
	step_motor.step_motor_move_type=move_type;
}

void Step_Motor_Step(void)
{
	step_motor.cycle_counter&=0x7;
	switch(step_motor.cycle_counter)
	{
		case 0:
		{
		    PHAZE_0=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_1=0;
		    PHAZE_2=0;
		    PHAZE_3=0;
		}
		break;

		case 1:
		{
		    PHAZE_0=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_1=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_2=0;
		    PHAZE_3=0;
		}
		break;

		case 2:
		{
		    PHAZE_0=0;
		    PHAZE_1=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_2=0;
		    PHAZE_3=0;
		}
		break;

		case 3:
		{
		    PHAZE_0=0;
		    PHAZE_1=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_2=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_3=0;
		}
		break;

		case 4:
		{
		    PHAZE_0=0;
		    PHAZE_1=0;
		    PHAZE_2=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_3=0;
		}
		break;

		case 5:
		{
		    PHAZE_0=0;
		    PHAZE_1=0;
		    PHAZE_2=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_3=STEP_MOTOR_ROTATE_PWM;
		}
		break;

		case 6:
		{
		    PHAZE_0=0;
		    PHAZE_1=0;
		    PHAZE_2=0;
		    PHAZE_3=STEP_MOTOR_ROTATE_PWM;
		}
		break;

		case 7:
		{
		    PHAZE_0=STEP_MOTOR_ROTATE_PWM;
		    PHAZE_1=0;
		    PHAZE_2=0;
		    PHAZE_3=STEP_MOTOR_ROTATE_PWM;
		}
		break;

		default:
		{
			step_motor.cycle_counter=0;
		}
		break;
	}
}

void Step_Motor_Step_Left(void)
{
	step_motor.cycle_counter++;
	Step_Motor_Step();
}

void Step_Motor_Step_Right(void)
{
	step_motor.cycle_counter--;
	Step_Motor_Step();
}

void Step_Motor_Hold(void)
{
	step_motor.cycle_counter&=0x7;
	switch(step_motor.cycle_counter)
	{
		case 0:
		{
		    PHAZE_0=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_1=0;
		    PHAZE_2=0;
		    PHAZE_3=0;
		}
		break;

		case 1:
		{
		    PHAZE_0=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_1=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_2=0;
		    PHAZE_3=0;
		}
		break;

		case 2:
		{
		    PHAZE_0=0;
		    PHAZE_1=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_2=0;
		    PHAZE_3=0;
		}
		break;

		case 3:
		{
		    PHAZE_0=0;
		    PHAZE_1=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_2=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_3=0;
		}
		break;

		case 4:
		{
		    PHAZE_0=0;
		    PHAZE_1=0;
		    PHAZE_2=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_3=0;
		}
		break;

		case 5:
		{
		    PHAZE_0=0;
		    PHAZE_1=0;
		    PHAZE_2=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_3=STEP_MOTOR_HOLDING_PWM;
		}
		break;

		case 6:
		{
		    PHAZE_0=0;
		    PHAZE_1=0;
		    PHAZE_2=0;
		    PHAZE_3=STEP_MOTOR_HOLDING_PWM;
		}
		break;

		case 7:
		{
		    PHAZE_0=STEP_MOTOR_HOLDING_PWM;
		    PHAZE_1=0;
		    PHAZE_2=0;
		    PHAZE_3=STEP_MOTOR_HOLDING_PWM;
		}
		break;

		default:
		{
			step_motor.cycle_counter=0;
		}
		break;
	}
}


