#include "gps.h"
#include "cmsis_os.h"
#include "change_status.h"
#include "mqtt.h"
#include "tim.h"

GPS_t gnss = {0};

extern uint32_t EC600U_REC_block_time;
extern uint32_t APP_Info_Submit_time;

extern osSemaphoreId_t APP_Info_Submit_SempHandle;
extern osSemaphoreId_t GPS_rec_exec_sempHandle;

extern osMessageQueueId_t usart1_recv_semp_queueHandle;

extern osEventFlagsId_t Device_Run_status_eventHandle;
extern osEventFlagsId_t Device_unusual_status_eventHandle;

/*GPS�豸�ϵ�*/
void  GPS_device_power()
{
	
}

/*���ڽ���GPS���ݴ���*/
//void USART_GPS_data(cJSON * object)
void SPI1_GPS_data(uint8_t *data)
{
	cJSON * GPS ={0};
	GPS = StringToObject((char *)data);
	if(cJSON_IsObject(GPS)&&(GPS != NULL ))
	{
		cJSON * Longitude      = cJSON_GetObjectItemCaseSensitive(GPS,"Longitude");
		cJSON * Latitude       = cJSON_GetObjectItemCaseSensitive(GPS,"Latitude");
		cJSON * Altitude       = cJSON_GetObjectItemCaseSensitive(GPS,"Altitude");
		cJSON * Mode           = cJSON_GetObjectItemCaseSensitive(GPS,"Mode");
		cJSON * UsedStateCnt   = cJSON_GetObjectItemCaseSensitive(GPS,"UsedStateCnt");
		cJSON * Hdop           = cJSON_GetObjectItemCaseSensitive(GPS,"Hdop");
		cJSON * CourseAngle    = cJSON_GetObjectItemCaseSensitive(GPS,"CourseAngle");
		cJSON * speed          = cJSON_GetObjectItemCaseSensitive(GPS,"speed");
		if(cJSON_IsString(Longitude) && cJSON_IsString(Latitude) && cJSON_IsString(Altitude) && cJSON_IsString(Mode) && cJSON_IsString(UsedStateCnt) && cJSON_IsString(Hdop) && cJSON_IsString(CourseAngle) && cJSON_IsString(speed))
		{
			/*�����ٽ���*/
			taskENTER_CRITICAL();

			/*ΪGPS��Ϣ��ֵ*/
			memset( &gnss, 0, sizeof( GPS_t )); //�ṹ���ڱ���ȫ����ʼ��Ϊ0
			strcpy(gnss.Lon ,          Longitude ->valuestring);
			strcpy(gnss.Lat ,          Latitude ->valuestring);
			strcpy(gnss.Altitude,      Altitude->valuestring);
			strcpy(gnss.Mode ,         Mode->valuestring);
			strcpy(gnss.UsedStateCnt , UsedStateCnt->valuestring);
			strcpy(gnss.Hdop ,         Hdop->valuestring);
			strcpy(gnss.CourseAngle ,  CourseAngle->valuestring);
			strcpy(gnss.speed ,        speed->valuestring);
			
			cJSON_Delete(GPS);
			/*�˳��ٽ���*/
			taskEXIT_CRITICAL();
			if(strcmp(gnss.Mode,"4") == 0)
			{
				osEventFlagsSet(Device_Run_status_eventHandle,BIT_5);
				if(osSemaphoreGetCount (GPS_rec_exec_sempHandle)<1)    osSemaphoreRelease(GPS_rec_exec_sempHandle);
			}
			else
			{
				osEventFlagsClear(Device_Run_status_eventHandle,BIT_5);
				osEventFlagsSet(Device_unusual_status_eventHandle,BIT_2);              //�����쳣 rtk��ʧ
			}
			if((osEventFlagsGet(Device_Run_status_eventHandle) &  BIT_4) ==  0)    // �Ѿ��յ�4Gģ�鷢���Ķ�λ��Ϣ ����û�����¼���־λ
			{
				osEventFlagsSet(Device_Run_status_eventHandle,BIT_4);                //����RTK_L1��־λ
				if(Device_Run_Status.Curstatus == Poweron)                 //ת��״̬Ϊ��ҵ�ȴ�
				{
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Job_Wait;
					EC600U_REC_block_time = portMAX_DELAY; 
					APP_Info_Submit_time = 8000;
					osSemaphoreRelease(APP_Info_Submit_SempHandle); //�ϴ�һ��APP��Ϣ ���ڽ���������ȴ�����ʱ���ٿ���MQTT
				}
				
				
			}
		}
	}
	else printf("GPS��Ϣ����\r\n");
}
