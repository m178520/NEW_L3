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

/*GPS设备上电*/
void  GPS_device_power()
{
	
}

/*串口接收GPS数据处理*/
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
			/*进入临界区*/
			taskENTER_CRITICAL();

			/*为GPS信息赋值*/
			memset( &gnss, 0, sizeof( GPS_t )); //结构体内变量全部初始化为0
			strcpy(gnss.Lon ,          Longitude ->valuestring);
			strcpy(gnss.Lat ,          Latitude ->valuestring);
			strcpy(gnss.Altitude,      Altitude->valuestring);
			strcpy(gnss.Mode ,         Mode->valuestring);
			strcpy(gnss.UsedStateCnt , UsedStateCnt->valuestring);
			strcpy(gnss.Hdop ,         Hdop->valuestring);
			strcpy(gnss.CourseAngle ,  CourseAngle->valuestring);
			strcpy(gnss.speed ,        speed->valuestring);
			
			cJSON_Delete(GPS);
			/*退出临界区*/
			taskEXIT_CRITICAL();
			if(strcmp(gnss.Mode,"4") == 0)
			{
				osEventFlagsSet(Device_Run_status_eventHandle,BIT_5);
				if(osSemaphoreGetCount (GPS_rec_exec_sempHandle)<1)    osSemaphoreRelease(GPS_rec_exec_sempHandle);
			}
			else
			{
				osEventFlagsClear(Device_Run_status_eventHandle,BIT_5);
				osEventFlagsSet(Device_unusual_status_eventHandle,BIT_2);              //触发异常 rtk丢失
			}
			if((osEventFlagsGet(Device_Run_status_eventHandle) &  BIT_4) ==  0)    // 已经收到4G模块发来的定位信息 但还没有置事件标志位
			{
				osEventFlagsSet(Device_Run_status_eventHandle,BIT_4);                //设置RTK_L1标志位
				if(Device_Run_Status.Curstatus == Poweron)                 //转换状态为作业等待
				{
					Device_Run_Status.Prestatus = Device_Run_Status.Curstatus;
					Device_Run_Status.Curstatus = Job_Wait;
					EC600U_REC_block_time = portMAX_DELAY; 
					APP_Info_Submit_time = 8000;
					osSemaphoreRelease(APP_Info_Submit_SempHandle); //上传一次APP信息 用于解除阻塞，等待以上时间再开启MQTT
				}
				
				
			}
		}
	}
	else printf("GPS消息出错\r\n");
}
