#include "level_gage_test.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdio.h"
#include "string.h"
#include "buzzer.h"


#include "watchdog.h"

//struct uks uks_channels;
//extern xSemaphoreHandle xMeasure_LM35_Semaphore;

//xTaskHandle UKS_Drying_Task_Handle;

extern struct task_watch task_watches[];

void Level_Gage_Test_Task(void *pvParameters );
uint8_t CRC_Check( uint8_t  *Spool_pr,uint8_t Count_pr );


void Level_Gage_Test_Init(void)
{
	uint8_t i=0,j=0;


}


void Level_Gage_Test_Task(void *pvParameters )
{
	uint8_t i=0;
	task_watches[DRYING_TASK].task_status=TASK_ACTIVE;
	while(1)
	{
//		if( xSemaphoreTake( xMeasure_LM35_Semaphore, ( portTickType ) portMAX_DELAY ) == pdTRUE )
////		vTaskDelay(1000);
//		{
//			//uks_channels.uks_params.delta_temp_start_drying+=1.0;
//			for(i=0;i<DRYING_CHANNELS_NUM;i++)
//			{
//				UKS_Channel_State_Drying(&uks_channels.drying_channel_list[i]);
//			}
//		}
		task_watches[DRYING_TASK].counter++;
	}
}



void Level_Gage_Test_Restore_Settings(void)
{

}


uint8_t  CRC_Check( uint8_t  *Spool_pr,uint8_t Count_pr )
{
	uint8_t crc_n = 0;
	uint8_t  *Spool;
	uint8_t  Count ;

	Spool=Spool_pr;
	Count=Count_pr;

  		while(Count!=0x0)
        {
	        crc_n = crc_n ^ (*Spool++);

	        crc_n = ((crc_n & 0x01) ? (uint8_t)0x80: (uint8_t)0x00) | (uint8_t)(crc_n >> 1);

	        if (crc_n & (uint8_t)0x80) crc_n = crc_n ^ (uint8_t)0x3C;
			Count--;
        }
    return crc_n;
}
