#include "uart_data.h"
#include "usart.h"
#include "change_status.h"

extern uint32_t EC600U_REC_block_time;

extern osMessageQueueId_t uart4_recv_semp_queueHandle;
extern osMessageQueueId_t uart4_send_semp_queueHandle;

extern osMessageQueueId_t VCU_recv_semp_queueHandle;
extern osMessageQueueId_t VCU_send_semp_queueHandle;

extern osMessageQueueId_t HTTP_REQUEST_queueHandle;
extern osEventFlagsId_t Device_Run_status_eventHandle;

char Imei[30];

/*����Imei*/
void USART_Imei_data(cJSON* object);

/*����1���ͺ���*/
osStatus_t uart4_send_data_apply(uint8_t *data,uint16_t len)
{
	osStatus_t status;
	
	/*�����ٽ���*/
		taskENTER_CRITICAL();
	memset(UART4TxData[UART4_fifo.usTxWrite],0,UART4_Max_Txbuf_size);
	memcpy(UART4TxData[UART4_fifo.usTxWrite],data,len);
	if (++UART4_fifo.usTxWrite >= UART4_fifo.usTxBufSize)
	{
		UART4_fifo.usTxWrite = 0;
	}
	/*�˳��ٽ���*/
		taskEXIT_CRITICAL();
	
	if (osMessageQueueGetCount(uart4_send_semp_queueHandle) < UART4_fifo.usTxBufSize)
	{
		status = osMessageQueuePut(uart4_send_semp_queueHandle,&len,0,10);
	}
	return  status;
}

/*���ڽ��պ���*/
void uart4_rec_data_apply(uint8_t *data,uint16_t len)
{
	uint8_t null_len = 0;
	uint32_t BIT = 0;
	cJSON * EC600U_MQTT_RECV_STATUS = NULL;
	cJSON * Name = NULL;
	
	for(int i = 0; i<=len ; i++)
	{
		if(i == len) return;
		if(*(data + i) == '{') 
		{
			EC600U_MQTT_RECV_STATUS = StringToObject((char *)(data + i));
			break;
		}
	}
	
	
	if(cJSON_IsObject(EC600U_MQTT_RECV_STATUS)&&(EC600U_MQTT_RECV_STATUS != NULL ))
	{
		Name = cJSON_GetObjectItemCaseSensitive(EC600U_MQTT_RECV_STATUS,"Name");
		if(cJSON_IsString(Name)&&(Name->valuestring != NULL ))	
		{
			/*�յ�IMEI*/
			/*���յ�Imei EC600����������� ���м�Ȩ*/
			if(strcmp(Name->valuestring,"Imei") == 0)
			{
				USART_Imei_data(EC600U_MQTT_RECV_STATUS);
				HAL_GPIO_WritePin(GPIOC,  GPIO_PIN_4, GPIO_PIN_RESET);
				while(HAL_GPIO_ReadPin(GPIOC,  GPIO_PIN_4) != GPIO_PIN_RESET);
				osEventFlagsSet(Device_Run_status_eventHandle,BIT_2);       //���������ɹ���־λ
				osEventFlagsSet(Device_Run_status_eventHandle,BIT_1);       //�����ϵ�ɹ���־λ
				/*��ʼ��Ȩ����*/
				Device_Poweron_status = Check_Authen;
				EC600U_REC_block_time = 8000;                         //��ʱ5�룬�ȴ���Ȩ��Ϣ�ظ�
				osMessageQueuePut(uart4_recv_semp_queueHandle,&null_len,0,10); //�������
				BIT = BIT_0;
				osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);//����HTTP���м�Ȩ����

			}
			/*��ʹ�ô��ڽ���GPS����*/
//			/*�յ�GPS����*/
//			else if(strcmp(Name->valuestring,"GPS") == 0)
//			{
//				USART_GPS_data(EC600U_MQTT_RECV_STATUS);
//			}
			
			/*�յ�MQTT����*/
			else if(strcmp(Name->valuestring,"MQTT") == 0)
			{
				USART_MQTT_data(EC600U_MQTT_RECV_STATUS);
			}
			/*�յ�HTTP����*/
			else if(strcmp(Name->valuestring,"HTTP") == 0)
			{
				/*�ж�HTTP��Ӧ*/
				USART_HTTP_data(EC600U_MQTT_RECV_STATUS);
			}
			else printf("Name����\r\n");
		}
		else printf("Name����\r\n");
	}
	else
	{
		printf("EC600U���ݴ���\r\n");
	}
	
	//ʹ����ɺ�һ��Ҫ�ͷ�object����
	cJSON_Delete(EC600U_MQTT_RECV_STATUS);
	//ʹ����һ��Ҫ�������������
}

/*EC600U���ݸ�ʽ��װ���Զ�����ʽ����*/
void EC600U_send_msg(char* Name,char* fun,char *Source,uint16_t len)
{
	uint8_t *grade = (uint8_t *)my_malloc((len + 100)  * sizeof(char),__FUNCTION__, __LINE__);
	insert_str(ST_EC600("\0","\0","\0"),(char *)grade,"%s%s%s",Name,fun,Source);
	uart4_send_data_apply(grade,strlen((char*)grade));
	my_free(grade,__FUNCTION__, __LINE__);
}

void USART_Imei_data(cJSON* object)
{
	cJSON* Msg;
	Msg = cJSON_GetObjectItemCaseSensitive(object,"msg");
	if(cJSON_IsString(Msg))
	{
		/*ΪImei��Ϣ��ֵ*/
		strcpy(Imei,Msg->valuestring);
	}
	else printf("Imei��Ϣ����\r\n");
}

///*����2���ͺ���*/
//osStatus_t usart2_send_data_apply(uint8_t *data,uint16_t len)
//{
//	osStatus_t status;
//	memset(USART2TxData[UART2_fifo.usTxWrite],0,USART2_Max_Txbuf_size);
//	memcpy(USART2TxData[UART2_fifo.usTxWrite],data,len);
//	if (++UART2_fifo.usTxWrite >= UART2_fifo.usTxBufSize)
//	{
//		UART2_fifo.usTxWrite = 0;
//	}
//	if (osMessageQueueGetCount(VCU_send_semp_queueHandle) < UART2_fifo.usTxBufSize)
//	{
//		status = osMessageQueuePut(VCU_send_semp_queueHandle,&len,0,10);
//	}
//	return  status;
//}
///*���ڽ��պ���*/
//void usart2_rec_data_apply(uint8_t *data,uint16_t len)
//{
//		if(strlen((char *)data) == 0)
//	{
//		return ;
//	}
//	
//}
