#include "mqtt.h"
#include "usart.h"
#include "spi.h"
#include "cmsis_os.h"
#include "main.h"
#include "tim.h"
#include "gps.h"


MQTT_Task_Msg_t MQT_Base_Msg = {0};
MQTT_Task_Msg_t MQTT_self_Base_Msg = {0};

int Knife_Height = 0;
int MQTT_Return_Task_ZoneId = 0;
int Safe_Zone_Detect = 0;
MQTT_Task_Msg_t MQTT_Task_Msg = {0};
char Change_Safe_Zone[100] = {0};
int  Light_Status =0;
int  Delete_Device=0;

char Device_MQTT_Status[] = DEVICE_MQTT_STATUS;

/*开启rtk使用*/
extern uint32_t GPS_REC_block_time;
extern osMessageQueueId_t SPI1_recv_semp_queueHandle;

MQTT_info_t MQTT_APP_info = {
	MQTT_APP_IP,
	MQTT_APP_PORT,
	MQTT_APP_NAME,
	MQTT_APP_PASSWORD,
	{0},
	{0}
};

extern cJSON * EC600U_MQTT_SEND_STATUS;
extern cJSON * EC600U_HTTP_jobPause;

extern uint32_t EC600U_REC_block_time;

extern osEventFlagsId_t Device_Run_status_eventHandle;
extern osMessageQueueId_t HTTP_REQUEST_queueHandle;

extern void EC600U_send_msg(char* Name,char* fun,char *Source,uint16_t len);
extern UART_HandleTypeDef huart1;

//格式：{"Name":"MQTT","fun":"APP_open","Source":{"IP":"dev.server.sdxinzuobiao.com","port":"1883","username":"wanmeng","password":"WanmengKeji","sub":"xzb/autopilot/mower/1/device/service/call/50/108}"}}
void MQTT_open(char *fun, char *ip,char *port,char *username,char *password,char *sub)
{
	char q[512] = {0};
	insert_str(CONN_MSG("\0","\0","\0","\0","\0"),q,"%s%s%s%s%s",ip,port,username,password,sub);
	EC600U_send_msg("MQTT",fun,q,MSG_SIZE);
}

//{"Name":"MQTT","fun":"APP_pub","Source":{"pub":"xzb/autopilot/mower/1/device/property/post/50/108","msg":{"num":"2"}}}
void MQTT_pub(char* pub,char *msg)
{
	char q[512] = {0};
	insert_str(PUB_MSG("\0","\0"),q,"%s%s",pub,msg);
	EC600U_send_msg("MQTT","APP_pub",q,MSG_SIZE);
}

/*APP_MQTT信息上报函数*/
void APP_Info_Submit()
{ 
	char *trans_Msg=NULL;
	char q[1300] = {0};
	/*修改上报信息值*/
	/*进入临界区*/
	taskENTER_CRITICAL();
	Json_data_Change(EC600U_MQTT_SEND_STATUS,"%f%s%s",strtod(gnss.Lon,NULL),"property","lon");
	Json_data_Change(EC600U_MQTT_SEND_STATUS,"%f%s%s",strtod(gnss.Lat,NULL),"property","lat");
	Json_data_Change(EC600U_MQTT_SEND_STATUS,"%f%s%s",strtod(gnss.CourseAngle,NULL),"property","yaw");
	/*退出临界区*/
	taskEXIT_CRITICAL();
	
	trans_Msg = ObjectToString(EC600U_MQTT_SEND_STATUS);
	
	if(trans_Msg != NULL)
	{
		/*修改上报MQTT的数据*/
//		insert_str(ST_EC600("\0","\0","\0"),q,"%s%s","MQTT","APP_pub");
		insert_str(PUB_MSG("\0","\0"),q,"%s%s",MQTT_APP_info.pub,trans_Msg);
//		HAL_UART_Transmit_DMA(&huart1,(uint8_t *)q,strlen(q));
		EC600U_send_msg("MQTT","APP_pub",q,strlen(q)+80);
//		osDelay(50);
		cJSON_free(trans_Msg);
	}
	else
	{
		printf("上报信息转换失败\r\n");
	}
}


/*串口接收MQTT数据处理*/
void USART_MQTT_data(cJSON * object)
{
	uint32_t BIT = 0;
	cJSON * fun ={0};
	cJSON * Msg ={0};
	cJSON * Type ={0};
	cJSON * Data ={0};
	Msg = cJSON_GetObjectItemCaseSensitive(object,"msg");
	if(cJSON_IsObject(Msg)&&(Msg != NULL ))
	{
		/*判断APP_MQTT消息命令*/
		Type = cJSON_GetObjectItemCaseSensitive(Msg,"type");
		if(cJSON_IsNumber(Type)&&(Type->valueint != NULL ))
		{
			printf("%x",Type->valueint);
			switch(Type->valueint)
			{
				case Delete_dev://删除设备
					
					break;
				case Start_dev: //启动任务
					Data = cJSON_GetObjectItemCaseSensitive(Msg,"data");
					if(cJSON_IsObject(Data)&&(Data != NULL ))
					{
						MQTT_Task_Msg.taskId = cJSON_GetObjectItemCaseSensitive(Data,"taskId")->valueint;
						MQTT_Task_Msg.zoneId = cJSON_GetObjectItemCaseSensitive(Data,"zoneId")->valueint;
						BIT = BIT_1;
						osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
					}
					break;
				case Pause_dev: //暂停任务
//					cJSON_GetObjectItemCaseSensitive(EC600U_HTTP_jobPause,"progress") ->valueint = progress;
					BIT = BIT_2;
					osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
					break;
				case Continue_dev: //继续任务
					
						MQTT_Task_Msg.taskId = cJSON_GetObjectItemCaseSensitive(Data,"taskId")->valueint;
						MQTT_Task_Msg.zoneId = cJSON_GetObjectItemCaseSensitive(Data,"zoneId")->valueint;
						BIT = BIT_3;		
						osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
					
					break;
				case Return_dev: //一键召回
					MQTT_Return_Task_ZoneId = cJSON_GetObjectItemCaseSensitive(Data,"zoneId")->valueint;
					BIT = BIT_6;	
					osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
					break;
				case 2005: //割刀高度
					Knife_Height = cJSON_GetObjectItemCaseSensitive(Data,"height")->valueint;
					break;
				case 2006: //安全区域检测
					Safe_Zone_Detect = cJSON_GetObjectItemCaseSensitive(Data,"enable")->valueint;
					break;
				case 2007: //修改安全区域检测
					strcpy(Change_Safe_Zone, cJSON_GetObjectItemCaseSensitive(Data,"safeZone")->valuestring);
					break;
				case 2008: //车灯控制
					Light_Status = cJSON_GetObjectItemCaseSensitive(Data,"status")->valueint;
					break;
				case 2009: //割刀控制
					
					break;
				default : printf("APP_type有误\r\n"); break;
			}
		}
	}
	else if(cJSON_IsString(Msg)&&(Msg ->valuestring != NULL ))         //如果Msg是一个字符串，则说明上报MQTT的状态
	{
		fun = cJSON_GetObjectItemCaseSensitive(object,"fun");
		if(cJSON_IsString(fun)&&(fun ->valuestring != NULL ))
		{
			//判断是哪一个MQTT的状态信息
			//来自APP的  
			if(strstr(fun ->valuestring,"APP"))                               
			{
				if(strstr(Msg ->valuestring,"success"))
				{
					uint8_t null_len = 0;
					printf("MQTT_APP打开成功\r\n");
					/*开始打开RTK操作*/
					Device_Poweron_status = Check_RTK;
					/*打开RTK*/
					HAL_SPI_Receive_DMA(&hspi1 , SPI1RxData[SPI1_fifo.usRxWrite] ,180 );
					RTK_L1_open("OPEN","Moving","\"dif_base\"");
					EC600U_REC_block_time = portMAX_DELAY;
					GPS_REC_block_time    = 1500;
					osMessageQueuePut(SPI1_recv_semp_queueHandle,&null_len,0,10);
					osEventFlagsSet(Device_Run_status_eventHandle,BIT_6);      //设置MQTT_APP开启成功标志位
				}
				else if(strstr(Msg ->valuestring,"fail"))
				{
					printf("MQTT_APP打开失败\r\n");
					HAL_TIM_Base_Stop_IT(&htim6);
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_6);    //设置MQTT_APP清除成功标志位
				}
			}
			//来自基站的
			else if(strcmp(fun ->valuestring,"Base_MQTT") == 0)                 
			{
				if(strstr(Msg ->valuestring,"success"))
				{
					printf("MQTT_Base打开成功\r\n");
					osEventFlagsSet(Device_Run_status_eventHandle,BIT_7);      //设置MQTT_APP开启成功标志位
					
				}
				else if(strstr(Msg ->valuestring,"fail"))
				{
					printf("MQTT_Base打开失败\r\n");
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_7);    //设置MQTT_APP开启成功标志位
				}
			}
			//来自自建基站的
			else if(strcmp(fun ->valuestring,"Base_self_MQTT") == 0)            
			{
				if(strstr(Msg ->valuestring,"success"))
				{
					printf("MQTT_self_Base打开成功\r\n");
					osEventFlagsSet(Device_Run_status_eventHandle,BIT_8);      //设置MQTT_APP开启成功标志位
				}
				else if(strstr(Msg ->valuestring,"fail"))
				{
					printf("MQTT_self_Base打开成功\r\n");
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_8);    //设置MQTT_APP开启成功标志位
				}
			}
			else printf("未知MQTT信息\r\n");
		}
	}
	else printf("MQTT消息出错\r\n");
}

