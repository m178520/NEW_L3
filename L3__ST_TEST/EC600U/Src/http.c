#include "http.h"
#include "cmsis_os.h"
#include "main.h"
#include "nav.h"
#include "gps.h"

Authen_info_t Authen_info ={0};
HTTP_task_t   HTTP_Task_Msg ={0};
Charge_info_t Charge_info={0};

char Update_Route_param[] = UPDATE_ROUTE_PARAM;
char Pause_param[] = PAUSE_PARAM;

extern char Imei[30];
extern osEventFlagsId_t Device_Run_status_eventHandle;
extern osEventFlagsId_t Device_unusual_status_eventHandle;
extern void EC600U_send_msg(char* Name,char* fun,char *Source,uint16_t len);

extern uint32_t EC600U_REC_block_time;

extern cJSON * EC600U_MQTT_SEND_STATUS;


extern osMessageQueueId_t uart4_recv_semp_queueHandle;
extern osMessageQueueId_t HTTP_REQUEST_queueHandle;


uint8_t request_num = 0; //���ڳ��ִ���ʱ���м�������δ��󱨴�

void HTTP_get(char* url,char* data,char *Header)
{
	char q[512] = {0};
	insert_str( HTTP_REQUEST_MSG("\0","\0","\0"),q,"%s%s%s",url,data,Header);
	EC600U_send_msg("HTTP","get",q,600);
}

void HTTP_post(char* url,char* data,char *Header)
{
	char q[512] = {0};
	insert_str( HTTP_REQUEST_MSG("\0","\0","\0"),q,"%s%s%s",url,data,Header);
	EC600U_send_msg("HTTP","post",q,600);
}

void HTTP_put(char* url,char* data,char *Header)
{
	char q[512] = {0};
	insert_str( HTTP_REQUEST_MSG("\0","\0","\0"),q,"%s%s%s",url,data,Header);
	EC600U_send_msg("HTTP","get",q,600);
}

/*��Ȩ��Ϣ����*/
void HTTP_Authen_Request(void)
{
	char checkSum[50];
	sprintf(checkSum,"%s%s","checkSum=",Imei);
	HTTP_post(AUTHEN_URL,checkSum,"{\"Content-Type\":\"application/x-www-form-urlencoded;charset=UTF-8\"}");
}

/*��ʼ������Ϣ����*/
void HTTP_jobStart_Request(void)
{
	char data[50];
	char Header[150];
	sprintf(data,"%s%d","zoneId=",MQTT_Task_Msg.zoneId);
	insert_str(HTTP_REQUEST_HEADER_MSG("\0","\0","\0"),Header,"%s%d%d", "application/x-www-form-urlencoded;charset=UTF-8" ,Authen_info.deviceId,Authen_info.groupId);
	HTTP_post(JOBSTART_URL,data,Header);
	printf("���Ϳ�ʼ����");
}

/*��ͣ������Ϣ����*/
void HTTP_jobPause_Request(void)
{
	char Header[150];
	char *trans_Msg;
	
	Json_data_Change(EC600U_HTTP_jobPause,"%d%s",strtod(gnss.Lat,NULL),"pauseLat"); 
	Json_data_Change(EC600U_HTTP_jobPause,"%d%s",strtod(gnss.Lon,NULL),"pauseLon");
	Json_data_Change(EC600U_HTTP_jobPause,"%d%s",waypoints_run_status.processed_allnum,"progress");
	Json_data_Change(EC600U_HTTP_jobPause,"%d%s",waypoints_run_status.current_toindex,"targetIndex");
	Json_data_Change(EC600U_HTTP_jobPause,"%d%s",HTTP_Task_Msg.zoneId,"zoneId");
	
	trans_Msg = ObjectToString(EC600U_HTTP_jobPause);
	if(trans_Msg != NULL)
	{
		insert_str(HTTP_REQUEST_HEADER_MSG("\0","\0","\0"),Header,"%s%d%d","application/json",Authen_info.deviceId,Authen_info.groupId);
		HTTP_post(JOBPAUSE_URL,trans_Msg,Header);
		cJSON_free(trans_Msg);
	}
}

/*����������Ϣ����*/
void HTTP_jobContinue_Request(void)
{
	char data[50];
	char Header[150];
	sprintf(data,"%s%d","zoneId=",MQTT_Task_Msg.zoneId);
	insert_str(HTTP_REQUEST_HEADER_MSG("\0","\0","\0"),Header,"%s%d%d", "application/x-www-form-urlencoded;charset=UTF-8" ,Authen_info.deviceId,Authen_info.groupId);
	HTTP_post(JOBCONTINUE_URL,data,Header);
}

/*��ɹ�����Ϣ����*/
void HTTP_jobFinish_Request(void)
{
	char data[50];
	char Header[150];
	sprintf(data,"%s%d%c%s%d","zoneId=",MQTT_Task_Msg.zoneId,'&',"pointNum=",HTTP_Task_Msg.taskNum);
	insert_str(HTTP_REQUEST_HEADER_MSG("\0","\0","\0"),Header,"%s%d%d", "application/x-www-form-urlencoded;charset=UTF-8" ,Authen_info.deviceId,Authen_info.groupId);
	HTTP_post(JOBFINISH_URL,data,Header);
}

/*��ȡ�ֶκ���*/
void HTTP_updateRoute_Request(void)
{
	char Header[150];
	char *trans_Msg;
	
	Json_data_Change(EC600U_HTTP_updateRoute,"%d%s",10,"process");  /* ��ʱδ�����ôд */
	Json_data_Change(EC600U_HTTP_updateRoute,"%d%s",10,"size");
	Json_data_Change(EC600U_HTTP_updateRoute,"%d%s",waypoints_run_status.Parse_num,"startIndex");
	Json_data_Change(EC600U_HTTP_updateRoute,"%d%s",waypoints_run_status.current_toindex,"tarIndex");
	Json_data_Change(EC600U_HTTP_updateRoute,"%d%s",HTTP_Task_Msg.taskId,"taskId");
	Json_data_Change(EC600U_HTTP_updateRoute,"%d%s",HTTP_Task_Msg.zoneId,"zoneId");
	
	trans_Msg = ObjectToString(EC600U_HTTP_updateRoute);
	if(trans_Msg != NULL)
	{
		insert_str(HTTP_REQUEST_HEADER_MSG("\0","\0","\0"),Header,"%s%d%d","application/json",Authen_info.deviceId,Authen_info.groupId);
		HTTP_post(UPDATEROUTE_URL,trans_Msg,Header);
		cJSON_free(trans_Msg);
	}
}

/*��ȡ���׮��Ϣ����*/
void HTTP_goToCharge_Request(void)
{
	char data[50];
	char Header[150];
	sprintf(data,"%s%d","zoneId=",MQTT_Task_Msg.zoneId);
	insert_str(HTTP_REQUEST_HEADER_MSG("\0","\0","\0"),Header,"%s%d%d", "application/x-www-form-urlencoded;charset=UTF-8" ,Authen_info.deviceId,Authen_info.groupId);
	HTTP_post(JOBCONTINUE_URL,data,Header);
}


/*���ڽ���HTTP���ݴ���*/
void USART_HTTP_data(cJSON * object)
{
	  uint32_t BIT = 0;
		cJSON * fun ={0};
		cJSON * Msg ={0};
		Msg = cJSON_GetObjectItemCaseSensitive(object,"msg");
		if(cJSON_IsObject(Msg)&&(Msg != NULL ))
		{
			/*��Ȩ����*/
			if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"authentication"))
			{
				USART_HTTP_Authen_data(Msg);
			}
			
			/*��ʼ��������*/
			else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"jobStart"))
			{
				USART_HTTP_jobStart_data(Msg);
			}
			
			/*��ͣ��������*/
			else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"jobPause"))
			{
				USART_HTTP_jobPause_data(Msg);
			}
				
			/*������������*/
			else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"jobContinue"))
			{
				USART_HTTP_jobContinue_data(Msg);
			}
			
			/*�����������*/
			else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"jobFinish"))
			{
				USART_HTTP_jobFinish_data(Msg);
			}
			
			/*��ȡ�ֶκ�������*/
			else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"updateRoute"))
			{
				USART_HTTP_updateRoute_data(Msg);
			}
			
			/*��ȡ���׮λ������*/
			else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"goToCharge"))
			{
				USART_HTTP_goToCharge_data(Msg);
			}
		}
		else if(cJSON_IsString(Msg)&&(Msg ->valuestring != NULL ))         //���Msg��һ���ַ�������˵���ϱ�MQTT��״̬
		{
			fun = cJSON_GetObjectItemCaseSensitive(object,"fun");
			if(cJSON_IsString(fun)&&(fun ->valuestring != NULL ))
			{
				if(strstr(Msg ->valuestring,"fail"))                                                       //http����ͨѶ��������
				{
					if(request_num <= 5)
					{
						/*��Ȩ����*/
						if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"authentication"))
						{
							printf("HTTP��Ȩ����ʧ��\r\n");
							BIT = BIT_0;
							osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
						}
						
						/*��ʼ��������*/
						else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"jobStart"))
						{
							printf("HTTP��Ȩ����ʧ��\r\n");
							BIT = BIT_1;
							osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
						}
						
						/*��ͣ��������*/
						else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"jobPause"))
						{
							printf("HTTP��ʼ��������ʧ��\r\n");
							BIT = BIT_2;
							osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
						}
							
						/*������������*/
						else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"jobContinue"))
						{
							printf("HTTP��������ʧ��\r\n");
							BIT = BIT_3;
							osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
						}
						
						/*�����������*/
						else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"jobFinish"))
						{
							printf("HTTP�������ʧ��\r\n");
							BIT = BIT_4;
							osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
						}
						
						/*��ȡ�ֶκ�������*/
						else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"updateRoute"))
						{
							printf("HTTP��ȡ�ֶκ���ʧ��\r\n");
							BIT = BIT_5;
							osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
						}
						
						/*��ȡ���׮λ������*/
						else if(strstr(cJSON_GetObjectItemCaseSensitive(object,"url")->valuestring,"goToCharge"))
						{
							printf("HTTP��ȡ���׮λ��ʧ��\r\n");
							BIT = BIT_6;
							osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
						}
						request_num ++;
					}
					else
					{
						request_num = 0;
						EC600U_REC_block_time = portMAX_DELAY;
						printf("HTTP��ȡʧ��");
					}
				}
			}
		}
		else printf("HTTP��Ϣ����\r\n");
}

/*����HTTP ��Ȩ����*/
void USART_HTTP_Authen_data(cJSON * object)
{
	uint8_t null_len = 0;
	cJSON * Status ={0};
	cJSON * Data ={0};
	/*�ж��Ƿ�����ɹ�*/
	Status = cJSON_GetObjectItemCaseSensitive(object,"code");
	if(cJSON_IsNumber(Status)&&Status->valueint == 100)
	{
		request_num = 0;
		Data = cJSON_GetObjectItemCaseSensitive(object,"data");
		if(cJSON_IsObject(Data)&&(Data != NULL ))
		{
			Authen_info.groupId = cJSON_GetObjectItemCaseSensitive(Data,"groupId")->valueint;
			Authen_info.deviceId = cJSON_GetObjectItemCaseSensitive(Data,"deviceId")->valueint;
			
			Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Authen_info.groupId,"property","gId");
			Json_data_Change(EC600U_MQTT_SEND_STATUS,"%d%s%s",Authen_info.deviceId,"property","dId");
			
			
			if( Authen_info.groupId != 0 || Authen_info.deviceId != 0)
			{
				printf("��Ȩ�ɹ�\r\n");
				/*��MQTTapp��sub��pub��ֵ*/
				memset(MQTT_APP_info.pub,0,100);
				memset(MQTT_APP_info.sub,0,100);
				insert_str(MQTT_PUB_TOPIC("\0","\0"),MQTT_APP_info.pub,"%d%d",Authen_info.groupId,Authen_info.deviceId);
				insert_str(MQTT_SUB_TOPIC("\0","\0"),MQTT_APP_info.sub,"%d%d",Authen_info.groupId,Authen_info.deviceId);
				osEventFlagsSet(Device_Run_status_eventHandle,BIT_3);      //���ü�Ȩ�ɹ���־λ
				/*��ʼ��MQTT_APP����*/
				Device_Poweron_status = Check_MQTT_APP;
				EC600U_REC_block_time = 8000;                                   //��ʱ5�룬�ȴ���Ȩ��Ϣ�ظ�
				osMessageQueuePut(uart4_recv_semp_queueHandle,&null_len,0,10); //�������
				/*����MQTT*/
				MQTT_open("APP_open",MQTT_APP_info.ip,MQTT_APP_info.port,MQTT_APP_info.username,MQTT_APP_info.password,MQTT_APP_info.sub); //��MQTT_APP
			}
		}
	}
	else  
	{
		osEventFlagsClear(Device_Run_status_eventHandle,BIT_3);      //���ü�Ȩʧ�ܱ�־λ
		printf("��Ȩ����ʧ�ܣ����豸δע��\r\n");
	}
}

/*����HTTP ��ʼ��������*/
void USART_HTTP_jobStart_data(cJSON * object)
{
	cJSON * Status ={0};
	cJSON * Data ={0};
	/*�ж��Ƿ�����ɹ�*/
	Status = cJSON_GetObjectItemCaseSensitive(object,"code");
	if(cJSON_IsNumber(Status)&&Status->valueint == 100)
	{
		request_num = 0;
		Data = cJSON_GetObjectItemCaseSensitive(object,"data");
		if(cJSON_IsObject(Data)&&(Data != NULL ))
		{
			HTTP_Task_Msg.zoneId      = cJSON_GetObjectItemCaseSensitive(Data,"zoneId")->valueint;
			HTTP_Task_Msg.taskId      = cJSON_GetObjectItemCaseSensitive(Data,"taskId")->valueint;
			strcpy(HTTP_Task_Msg.waypoints, cJSON_GetObjectItemCaseSensitive(Data,"waypoints")->valuestring);
			HTTP_Task_Msg.targetIndex = cJSON_GetObjectItemCaseSensitive(Data,"targetIndex")->valueint;
			HTTP_Task_Msg.offSet      = cJSON_GetObjectItemCaseSensitive(Data,"offSet")->valueint;
			HTTP_Task_Msg.taskNum     = cJSON_GetObjectItemCaseSensitive(Data,"taskNum")->valueint;
		}
		/*����״̬�任*/
		Device_Run_Status.Alterstatus = Job_Working;
		osEventFlagsSet(Device_unusual_status_eventHandle,BIT_1);              //����״̬�任
	}
	else  printf("����ʧ��\r\n");
}

void USART_HTTP_jobContinue_data(cJSON * object)
{
	cJSON * Status ={0};
	cJSON * Data ={0};
		/*�ж��Ƿ�����ɹ�*/
		Status = cJSON_GetObjectItemCaseSensitive(object,"code");
		if(cJSON_IsNumber(Status)&&Status->valueint == 100)
		{
				request_num = 0;
				Data = cJSON_GetObjectItemCaseSensitive(object,"data");
				if(cJSON_IsObject(Data)&&(Data != NULL ))
				{
//					HTTP_Task_Msg.zoneId      = cJSON_GetObjectItemCaseSensitive(Data,"zoneId")->valueint;
//					HTTP_Task_Msg.taskId      = cJSON_GetObjectItemCaseSensitive(Data,"taskId")->valueint;
					strcpy(HTTP_Task_Msg.waypoints, cJSON_GetObjectItemCaseSensitive(Data,"waypoints")->valuestring);
					HTTP_Task_Msg.targetIndex = cJSON_GetObjectItemCaseSensitive(Data,"targetIndex")->valueint;
//					HTTP_Task_Msg.offSet      = cJSON_GetObjectItemCaseSensitive(Data,"offSet")->valueint;
//					HTTP_Task_Msg.taskNum     = cJSON_GetObjectItemCaseSensitive(Data,"taskNum")->valueint;
					
					/*����״̬�任*/
					Device_Run_Status.Alterstatus = Job_Working;
					osEventFlagsSet(Device_unusual_status_eventHandle,BIT_1);              //����״̬�任
				}
		}
		else  printf("����ʧ��\r\n");
}

/*����HTTP ��ͣ��������*/
void USART_HTTP_jobPause_data(cJSON * object)
{
	cJSON * Status ={0};
	/*�ж��Ƿ�����ɹ�*/
	Status = cJSON_GetObjectItemCaseSensitive(object,"code");
	if(cJSON_IsNumber(Status)&&Status->valueint == 100)
	{
		request_num = 0;

		/*����״̬�任*/
		Device_Run_Status.Alterstatus = Job_Pause;
		osEventFlagsSet(Device_unusual_status_eventHandle,BIT_1);              //����״̬�任
	}
	else  printf("����ʧ��\r\n");
}

/*����HTTP �����������*/
void USART_HTTP_jobFinish_data(cJSON * object)
{
	cJSON * Status ={0};
	/*�ж��Ƿ�����ɹ�*/
	Status = cJSON_GetObjectItemCaseSensitive(object,"code");
	if(cJSON_IsNumber(Status)&&Status->valueint == 100)
	{
		request_num = 0;

		/*����״̬�任*/
		Device_Run_Status.Alterstatus = Job_Wait;
		osEventFlagsSet(Device_unusual_status_eventHandle,BIT_1);              //����״̬�任
	}
	else  printf("����ʧ��\r\n");
}

void USART_HTTP_updateRoute_data(cJSON * object)
{
	cJSON * Status ={0};
	cJSON * Data ={0};
	/*�ж��Ƿ�����ɹ�*/
	Status = cJSON_GetObjectItemCaseSensitive(object,"code");
	if(cJSON_IsNumber(Status)&&Status->valueint == 100)
	{
			request_num = 0;
			Data = cJSON_GetObjectItemCaseSensitive(object,"data");
			if(cJSON_IsObject(Data)&&(Data != NULL ))
			{
//					HTTP_Task_Msg.zoneId      = cJSON_GetObjectItemCaseSensitive(Data,"zoneId")->valueint;
//					HTTP_Task_Msg.taskId      = cJSON_GetObjectItemCaseSensitive(Data,"taskId")->valueint;
				strcpy(HTTP_Task_Msg.waypoints, cJSON_GetObjectItemCaseSensitive(Data,"waypoints")->valuestring);
				HTTP_Task_Msg.targetIndex = cJSON_GetObjectItemCaseSensitive(Data,"targetIndex")->valueint;
//					HTTP_Task_Msg.offSet      = cJSON_GetObjectItemCaseSensitive(Data,"offSet")->valueint;
//					HTTP_Task_Msg.taskNum     = cJSON_GetObjectItemCaseSensitive(Data,"taskNum")->valueint;
				waypoints_Parse(HTTP_Task_Msg.waypoints,",");  /*������յ��ĺ���*/
			}
	}
	else  printf("����ʧ��\r\n");
}

void USART_HTTP_goToCharge_data(cJSON * object)
{
	cJSON * Status ={0};
	cJSON * Data ={0};
	/*�ж��Ƿ�����ɹ�*/
	Status = cJSON_GetObjectItemCaseSensitive(object,"code");
	if(cJSON_IsNumber(Status)&&Status->valueint == 100)
	{
			request_num = 0;
			Data = cJSON_GetObjectItemCaseSensitive(object,"data");
			if(cJSON_IsObject(Data)&&(Data != NULL ))
			{
				Charge_info.chargeId = cJSON_GetObjectItemCaseSensitive(Data,"chargeId")->valueint;
				strcpy(Charge_info.navWaypoints, cJSON_GetObjectItemCaseSensitive(Data,"chargeId")->valuestring);
				
				/*����״̬�任*/
			Device_Run_Status.Alterstatus = Job_Return;
			osEventFlagsSet(Device_unusual_status_eventHandle,BIT_1);              //����״̬�任
			}
	}
	else  printf("����ʧ��\r\n");
}

