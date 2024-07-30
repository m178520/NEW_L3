/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "fdcan.h"
#include "tim.h"
#include "spi.h"

#include "sbus.h"
#include "nav.h"
#include "motor.h"
#include "uart_data.h"
#include "change_status.h"

extern uint8_t power_count; //用于EC600U上电记次数使用
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

cJSON * EC600U_MQTT_SEND_STATUS;
cJSON * EC600U_HTTP_jobPause;
cJSON * EC600U_HTTP_updateRoute;

uint32_t GPS_REC_block_time    = portMAX_DELAY;
uint32_t EC600U_REC_block_time = portMAX_DELAY;
uint32_t APP_Info_Submit_time  = portMAX_DELAY;
uint32_t Device_unusual_time   = portMAX_DELAY;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Power_Check */
osThreadId_t Power_CheckHandle;
const osThreadAttr_t Power_Check_attributes = {
  .name = "Power_Check",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow6,
};
/* Definitions for GPS_REC */
osThreadId_t GPS_RECHandle;
const osThreadAttr_t GPS_REC_attributes = {
  .name = "GPS_REC",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for SBUS_Parse */
osThreadId_t SBUS_ParseHandle;
const osThreadAttr_t SBUS_Parse_attributes = {
  .name = "SBUS_Parse",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for EC600U_REC */
osThreadId_t EC600U_RECHandle;
const osThreadAttr_t EC600U_REC_attributes = {
  .name = "EC600U_REC",
  .stack_size = 600 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for HTTP_REQUEST */
osThreadId_t HTTP_REQUESTHandle;
const osThreadAttr_t HTTP_REQUEST_attributes = {
  .name = "HTTP_REQUEST",
  .stack_size = 400 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for APP_Info_Submit */
osThreadId_t APP_Info_SubmitHandle;
const osThreadAttr_t APP_Info_Submit_attributes = {
  .name = "APP_Info_Submit",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityLow7,
};
/* Definitions for Device_Run */
osThreadId_t Device_RunHandle;
const osThreadAttr_t Device_Run_attributes = {
  .name = "Device_Run",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for Device_unusual */
osThreadId_t Device_unusualHandle;
const osThreadAttr_t Device_unusual_attributes = {
  .name = "Device_unusual",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for EC600U_SEND */
osThreadId_t EC600U_SENDHandle;
const osThreadAttr_t EC600U_SEND_attributes = {
  .name = "EC600U_SEND",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow4,
};
/* Definitions for VCU_send */
osThreadId_t VCU_sendHandle;
const osThreadAttr_t VCU_send_attributes = {
  .name = "VCU_send",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow7,
};
/* Definitions for uart5_recv_semp_queue */
osMessageQueueId_t uart5_recv_semp_queueHandle;
const osMessageQueueAttr_t uart5_recv_semp_queue_attributes = {
  .name = "uart5_recv_semp_queue"
};
/* Definitions for uart4_send_semp_queue */
osMessageQueueId_t uart4_send_semp_queueHandle;
const osMessageQueueAttr_t uart4_send_semp_queue_attributes = {
  .name = "uart4_send_semp_queue"
};
/* Definitions for uart4_recv_semp_queue */
osMessageQueueId_t uart4_recv_semp_queueHandle;
const osMessageQueueAttr_t uart4_recv_semp_queue_attributes = {
  .name = "uart4_recv_semp_queue"
};
/* Definitions for HTTP_REQUEST_queue */
osMessageQueueId_t HTTP_REQUEST_queueHandle;
const osMessageQueueAttr_t HTTP_REQUEST_queue_attributes = {
  .name = "HTTP_REQUEST_queue"
};
/* Definitions for VCU_send_semp_queue */
osMessageQueueId_t VCU_send_semp_queueHandle;
const osMessageQueueAttr_t VCU_send_semp_queue_attributes = {
  .name = "VCU_send_semp_queue"
};
/* Definitions for SPI1_recv_semp_queue */
osMessageQueueId_t SPI1_recv_semp_queueHandle;
const osMessageQueueAttr_t SPI1_recv_semp_queue_attributes = {
  .name = "SPI1_recv_semp_queue"
};
/* Definitions for APP_Info_Submit_Semp */
osSemaphoreId_t APP_Info_Submit_SempHandle;
const osSemaphoreAttr_t APP_Info_Submit_Semp_attributes = {
  .name = "APP_Info_Submit_Semp"
};
/* Definitions for GPS_rec_exec_semp */
osSemaphoreId_t GPS_rec_exec_sempHandle;
const osSemaphoreAttr_t GPS_rec_exec_semp_attributes = {
  .name = "GPS_rec_exec_semp"
};
/* Definitions for SBUS_RUN_Semp */
osSemaphoreId_t SBUS_RUN_SempHandle;
const osSemaphoreAttr_t SBUS_RUN_Semp_attributes = {
  .name = "SBUS_RUN_Semp"
};
/* Definitions for CAN_send_semp */
osSemaphoreId_t CAN_send_sempHandle;
const osSemaphoreAttr_t CAN_send_semp_attributes = {
  .name = "CAN_send_semp"
};
/* Definitions for Device_Run_status_event */
osEventFlagsId_t Device_Run_status_eventHandle;
const osEventFlagsAttr_t Device_Run_status_event_attributes = {
  .name = "Device_Run_status_event"
};
/* Definitions for Device_unusual_status_event */
osEventFlagsId_t Device_unusual_status_eventHandle;
const osEventFlagsAttr_t Device_unusual_status_event_attributes = {
  .name = "Device_unusual_status_event"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Power_Check_task(void *argument);
void GPS_REC_task(void *argument);
void SBUS_Parse_task(void *argument);
void EC600U_REC_task(void *argument);
void HTTP_REQUEST_task(void *argument);
void APP_Info_Submit_task(void *argument);
void Device_Run_task(void *argument);
void Device_unusual_task(void *argument);
void EC600U_SEND_task(void *argument);
void VCU_send_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of APP_Info_Submit_Semp */
  APP_Info_Submit_SempHandle = osSemaphoreNew(1, 0, &APP_Info_Submit_Semp_attributes);

  /* creation of GPS_rec_exec_semp */
  GPS_rec_exec_sempHandle = osSemaphoreNew(1, 0, &GPS_rec_exec_semp_attributes);

  /* creation of SBUS_RUN_Semp */
  SBUS_RUN_SempHandle = osSemaphoreNew(15, 0, &SBUS_RUN_Semp_attributes);

  /* creation of CAN_send_semp */
  CAN_send_sempHandle = osSemaphoreNew(15, 0, &CAN_send_semp_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of uart5_recv_semp_queue */
  uart5_recv_semp_queueHandle = osMessageQueueNew (30, sizeof(uint16_t), &uart5_recv_semp_queue_attributes);

  /* creation of uart4_send_semp_queue */
  uart4_send_semp_queueHandle = osMessageQueueNew (15, sizeof(uint16_t), &uart4_send_semp_queue_attributes);

  /* creation of uart4_recv_semp_queue */
  uart4_recv_semp_queueHandle = osMessageQueueNew (15, sizeof(uint16_t), &uart4_recv_semp_queue_attributes);

  /* creation of HTTP_REQUEST_queue */
  HTTP_REQUEST_queueHandle = osMessageQueueNew (16, sizeof(uint16_t), &HTTP_REQUEST_queue_attributes);

  /* creation of VCU_send_semp_queue */
  VCU_send_semp_queueHandle = osMessageQueueNew (15, sizeof(uint16_t), &VCU_send_semp_queue_attributes);

  /* creation of SPI1_recv_semp_queue */
  SPI1_recv_semp_queueHandle = osMessageQueueNew (15, sizeof(uint16_t), &SPI1_recv_semp_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Power_Check */
  Power_CheckHandle = osThreadNew(Power_Check_task, NULL, &Power_Check_attributes);

  /* creation of GPS_REC */
  GPS_RECHandle = osThreadNew(GPS_REC_task, NULL, &GPS_REC_attributes);

  /* creation of SBUS_Parse */
  SBUS_ParseHandle = osThreadNew(SBUS_Parse_task, NULL, &SBUS_Parse_attributes);

  /* creation of EC600U_REC */
  EC600U_RECHandle = osThreadNew(EC600U_REC_task, NULL, &EC600U_REC_attributes);

  /* creation of HTTP_REQUEST */
  HTTP_REQUESTHandle = osThreadNew(HTTP_REQUEST_task, NULL, &HTTP_REQUEST_attributes);

  /* creation of APP_Info_Submit */
  APP_Info_SubmitHandle = osThreadNew(APP_Info_Submit_task, NULL, &APP_Info_Submit_attributes);

  /* creation of Device_Run */
  Device_RunHandle = osThreadNew(Device_Run_task, NULL, &Device_Run_attributes);

  /* creation of Device_unusual */
  Device_unusualHandle = osThreadNew(Device_unusual_task, NULL, &Device_unusual_attributes);

  /* creation of EC600U_SEND */
  EC600U_SENDHandle = osThreadNew(EC600U_SEND_task, NULL, &EC600U_SEND_attributes);

  /* creation of VCU_send */
  VCU_sendHandle = osThreadNew(VCU_send_task, NULL, &VCU_send_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of Device_Run_status_event */
  Device_Run_status_eventHandle = osEventFlagsNew(&Device_Run_status_event_attributes);

  /* creation of Device_unusual_status_event */
  Device_unusual_status_eventHandle = osEventFlagsNew(&Device_unusual_status_event_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
//	uint32_t uxBits;
  /* Infinite loop */
  for(;;)
  {
//		uxBits = osEventFlagsWait(Device_Run_status_eventHandle,  BIT_1, osFlagsNoClear, portMAX_DELAY);
		
//		printf("run\r\n");
    osDelay(8000);
//		HTTP_updateRoute_Request();
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Power_Check_task */
/**
* @brief Function implementing the Power_Check thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Power_Check_task */
void Power_Check_task(void *argument)
{
  /* USER CODE BEGIN Power_Check_task */
  /* Infinite loop */
  for(;;)
  {
		if(power_count == 0)	
		{
			if(control_powerFlag == 1)
			{
				HAL_GPIO_WritePin(GPIOE,  GPIO_PIN_2, GPIO_PIN_SET);
				while(HAL_GPIO_ReadPin(GPIOE,  GPIO_PIN_2) != GPIO_PIN_SET);
				osDelay(8000);
				//开启串口空闲DMA空闲中断
				HAL_UARTEx_ReceiveToIdle_DMA(&huart4, UART4RxData[UART4_fifo.usRxWrite],UART4_Max_Rxbuf_size);
				HAL_SPI_Receive_DMA(&hspi1 , SPI1RxData[SPI1_fifo.usRxWrite] ,180 );
			  HAL_GPIO_WritePin(GPIOC,  GPIO_PIN_4, GPIO_PIN_SET);
				while(HAL_GPIO_ReadPin(GPIOC,  GPIO_PIN_4) != GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOC,  GPIO_PIN_4, GPIO_PIN_SET);
				while(HAL_GPIO_ReadPin(GPIOC,  GPIO_PIN_4) != GPIO_PIN_SET);
				osDelay(5000);
				HAL_GPIO_WritePin(GPIOC,  GPIO_PIN_4, GPIO_PIN_RESET);
				while(HAL_GPIO_ReadPin(GPIOC,  GPIO_PIN_4) != GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOE,  GPIO_PIN_2, GPIO_PIN_RESET);
				while(HAL_GPIO_ReadPin(GPIOE,  GPIO_PIN_2) != GPIO_PIN_RESET);
			}
			power_count++;
		}
    osDelay(100);
  }
  /* USER CODE END Power_Check_task */
}

/* USER CODE BEGIN Header_GPS_REC_task */
/**
* @brief Function implementing the GPS_REC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_GPS_REC_task */
void GPS_REC_task(void *argument)
{
  /* USER CODE BEGIN GPS_REC_task */
	osStatus_t err;
	uint32_t len = 0;
	
  /* Infinite loop */
  for(;;)
  {
		err = osMessageQueueGet (SPI1_recv_semp_queueHandle, &len, 0, GPS_REC_block_time); //调试阶段未使用，正式阶段将阻塞时间设置为1000
		if(err == osOK)
		{
			if(len != 0)
			{
//				printf("%s\r\n",SPI1RxData[SPI1_fifo.usRxRead]);
				
				SPI1_GPS_data(SPI1RxData[SPI1_fifo.usRxRead]);
				
				memset(SPI1RxData[SPI1_fifo.usRxRead],0,SPI1_Max_Rxbuf_size);
				if (++SPI1_fifo.usRxRead >= SPI1_fifo.usRxBufSize)
				{
					SPI1_fifo.usRxRead = 0;
				}
			}
			else  //说明只是为了来解除阻塞，以此判断是否超时
				printf("解除阻塞\r\n");
		}
		else
		{
//			if(Device_Poweron_status == Check_RTK || Device_Run_Status.Curstatus != Poweron)
//			{
				printf("接受RTK信息失败\r\n");
				HAL_TIM_Base_Stop_IT(&htim6);
				osEventFlagsClear(Device_Run_status_eventHandle,BIT_4);                //设置RTK消息丢失标志
				osEventFlagsClear(Device_Run_status_eventHandle,BIT_5);                //设置RTK消息丢失标志
				osEventFlagsSet(Device_unusual_status_eventHandle,BIT_2);              //触发异常
//			}
		}
    osDelay(1);
  }
  /* USER CODE END GPS_REC_task */
}

/* USER CODE BEGIN Header_SBUS_Parse_task */
/**
* @brief Function implementing the SBUS_Parse thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SBUS_Parse_task */
void SBUS_Parse_task(void *argument)
{
  /* USER CODE BEGIN SBUS_Parse_task */
	osStatus err;
	uint16_t Recv_Len;
	HAL_UARTEx_ReceiveToIdle_DMA(&huart5, UART5RxData[UART5_fifo.usRxWrite],UART5_Max_Rxbuf_size);
  /* Infinite loop */
  for(;;)
  {
		err = osMessageQueueGet (uart5_recv_semp_queueHandle, &Recv_Len, 0, 100);
		if(err == osOK)
		{
//			printf("%#x\r\n",UART5RxData[UART5_fifo.usRxRead][0]); //调试使用
			
			sbus_parse(UART5RxData[UART5_fifo.usRxRead],Recv_Len);
			
			memset(&UART5RxData[UART5_fifo.usRxRead],0,Recv_Len);
			if (++UART5_fifo.usRxRead >= UART5_fifo.usRxBufSize)
			{
				UART5_fifo.usRxRead = 0;
			}
		}
		else
		{
			SBUS_CH.Start = 0;
//			printf("SBUS信号丢失\r\n");
		}
		osDelay(1);
  }
  /* USER CODE END SBUS_Parse_task */
}

/* USER CODE BEGIN Header_EC600U_REC_task */
/**
* @brief Function implementing the EC600U_REC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_EC600U_REC_task */
void EC600U_REC_task(void *argument)
{
  /* USER CODE BEGIN EC600U_REC_task */
		osStatus_t err;
	uint16_t Recv_Len;
	uint32_t BIT = 0;
  /* Infinite loop */
  for(;;)
  {
		err = osMessageQueueGet (uart4_recv_semp_queueHandle, &Recv_Len, 0, EC600U_REC_block_time);
		if(err == osOK)
		{
			if(Recv_Len != 0) //说明串口发送来的数据
			{
//				printf("%s\r\n",UART4RxData[UART4_fifo.usRxRead]);
				uart4_rec_data_apply(UART4RxData[UART4_fifo.usRxRead],Recv_Len);
		
				memset(UART4RxData[UART4_fifo.usRxRead],0,Recv_Len);
				if (++UART4_fifo.usRxRead >= UART4_fifo.usRxBufSize)
				{
					UART4_fifo.usRxRead = 0;
				}
			}
			else  //说明只是为了来解除阻塞，以此判断是否超时
				printf("解除阻塞\r\n");
		}
				else // 超时处理
		{
			printf("CHAOSHI");
			if(Device_Run_Status.Curstatus == Poweron) //开机过程回复超时
			{
				if(Device_Poweron_status == Check_poweron)
				{
					printf("4G模块上电失败");
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_1);                //设置4G模块上电失败标志
					EC600U_REC_block_time = portMAX_DELAY;
				}
				else if(Device_Poweron_status == Check_Authen)
				{
					if((osEventFlagsGet(Device_Run_status_eventHandle) & BIT_3) != 0)    //网络好时请求HTTP超时，进行重复请求，网络不好时继续等待，等待网络良好时再进行操作
					{
						if(request_num <= 5)
						{
							printf("HTTP鉴权请求失败\r\n");
							BIT= BIT_0;
							osMessageQueuePut(HTTP_REQUEST_queueHandle, &BIT , 0 ,10);
							request_num ++;
						}
						else
						{
							printf("鉴权失败");
							osEventFlagsClear(Device_Run_status_eventHandle,BIT_3);                //设置鉴权失败标志
							EC600U_REC_block_time = portMAX_DELAY;
						}
					}
				}
				else if(Device_Poweron_status == Check_MQTT_APP)
				{
					printf("接受MQTT信息失败");
					osEventFlagsClear(Device_Run_status_eventHandle,BIT_6);                //设置RTK消息丢失标志
				}
			}
		}
    osDelay(1);
  }
  /* USER CODE END EC600U_REC_task */
}

/* USER CODE BEGIN Header_HTTP_REQUEST_task */
/**
* @brief Function implementing the HTTP_REQUEST thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_HTTP_REQUEST_task */
void HTTP_REQUEST_task(void *argument)
{
  /* USER CODE BEGIN HTTP_REQUEST_task */
	uint32_t Device_Run_Bits;
	uint32_t uxBits = 0;
	osStatus_t err;
	EC600U_HTTP_updateRoute = StringToObject(Update_Route_param);
	EC600U_HTTP_jobPause    = StringToObject(Pause_param);
  /* Infinite loop */
  for(;;)
  {
		Device_Run_Bits = osEventFlagsWait(Device_Run_status_eventHandle,BIT_2,osFlagsNoClear, portMAX_DELAY);
		err = osMessageQueueGet (HTTP_REQUEST_queueHandle, &uxBits, 0, portMAX_DELAY);
		if(err == osOK)
		{
			if((Device_Run_Bits & BIT_2)  != 0)
			{
				switch(uxBits)
				{
					case BIT_0 : HTTP_Authen_Request();      break;    /*鉴权指令*/
					case BIT_1 : HTTP_jobStart_Request();    break;    /*开始作业指令*/
					case BIT_2 : HTTP_jobPause_Request();    break;    /*暂停作业指令*/
					case BIT_3 : HTTP_jobContinue_Request(); break;    /*继续作业指令*/
					case BIT_4 : HTTP_jobFinish_Request();   break;    /*任务完成指令*/
					case BIT_5 : HTTP_updateRoute_Request(); break;    /*获取分段航点指令*/
					case BIT_6 : HTTP_goToCharge_Request();  break;    /*获取充电桩位置指令*/
					default: break;
				}
			}
		}
    osDelay(1);
  }
  /* USER CODE END HTTP_REQUEST_task */
}

/* USER CODE BEGIN Header_APP_Info_Submit_task */
/**
* @brief Function implementing the APP_Info_Submit thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_APP_Info_Submit_task */
void APP_Info_Submit_task(void *argument)
{
  /* USER CODE BEGIN APP_Info_Submit_task */
	osStatus_t err;
	EC600U_MQTT_SEND_STATUS = StringToObject(DEVICE_MQTT_STATUS);
  /* Infinite loop */
  for(;;)
  {
		err = osSemaphoreAcquire (APP_Info_Submit_SempHandle, APP_Info_Submit_time);
		if(err == osOK)
		{
			if((osEventFlagsGet(Device_Run_status_eventHandle) & BIT_6) != 0)
			{
				APP_Info_Submit();
			}
		}
		else
		{
			APP_Info_Submit_time  = portMAX_DELAY;
			//刚开始接受到gps数据，不稳定，等待一段时间后再开启上传
			HAL_TIM_Base_Start_IT(&htim6);                             //MQTT_APP打开成功并接收到GPS数据，开启APP信息上报
		}
		osDelay(1);
  }
  /* USER CODE END APP_Info_Submit_task */
}

/* USER CODE BEGIN Header_Device_Run_task */
/**
* @brief Function implementing the Device_Run thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device_Run_task */
void Device_Run_task(void *argument)
{
  /* USER CODE BEGIN Device_Run_task */
	uint32_t uxBits;
	osStatus_t err;
	NAV_output_t NAV_output = {0};
	CAN_Msg_t    CAN_Msg = {0};
  /* Infinite loop */
  for(;;)
  {
    if(controlFlag == NALCont)
		{
			uxBits = osEventFlagsWait(Device_Run_status_eventHandle,  BIT_1 | BIT_3 | BIT_4 | BIT_5 | BIT_23,osFlagsWaitAll | osFlagsNoClear, 1000); //还有bit0暂时先不加入BIT_0 |  等待1s（暂时）打印不满足的条件，如果转换了sbus控制就转换模式
			if( (uxBits & ( BIT_1 | BIT_3 | BIT_4 | BIT_5 | BIT_23))  == ( BIT_1 | BIT_3 | BIT_4 | BIT_5 | BIT_23)) //是否满足启动的条件
			{
				if(Device_Run_Status.Curstatus == Job_Working || Device_Run_Status.Curstatus == Job_Return)                                                      //是否完成了可工作的准备
				{
					err = osSemaphoreAcquire (GPS_rec_exec_sempHandle, 100);
					if(err == osOK)
					{
						NAV_output = NAV_Control();
//					printf("左轮：%f,右轮：%f\r\n",NAV_output.LSpeed,NAV_output.RSpeed);
						CAN_Msg = Direct_Drive_motor(NAV_output.RSpeed,NAV_output.LSpeed);
						CAN1_send_data_apply(CAN_Msg.L_Msg);
						CAN1_send_data_apply(CAN_Msg.R_Msg);
						HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);	
//					osDelay(20);
					}
				}
			}
			else //长时间不满足条件
			{
				printf("自动导航条件不满足");
			}
		}
		else if(controlFlag == sbusCont)
		{
			err = osSemaphoreAcquire (SBUS_RUN_SempHandle, 1000);
			if(err == osOK)
			{
				CAN_Msg = Direct_Drive_motor(SBUS_CH.RSpeed,SBUS_CH.LSpeed);
				CAN1_send_data_apply(CAN_Msg.L_Msg);
				CAN1_send_data_apply(CAN_Msg.R_Msg);
			}
		}
		osDelay(1);
  }
  /* USER CODE END Device_Run_task */
}

/* USER CODE BEGIN Header_Device_unusual_task */
/**
* @brief Function implementing the Device_unusual thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Device_unusual_task */
void Device_unusual_task(void *argument)
{
  /* USER CODE BEGIN Device_unusual_task */
	uint32_t uxBits;
  /* Infinite loop */
  for(;;)
  {
		uxBits = osEventFlagsWait(Device_unusual_status_eventHandle, BIT_0 | BIT_1 ,osFlagsWaitAny, Device_unusual_time);
		if(uxBits == (uint32_t)osErrorTimeout) //如果超时 事件标志位超时得到的为-2，但是为uint32格式 实际为0xFFFFFFFE
		{
			osEventFlagsSet(Device_Run_status_eventHandle,BIT_23);                //转变为启动
		}
		else if( (uxBits & BIT_0)  != 0 )               //强制停止
		{
			osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_0);
			osDelay(1000);
		}
		else if( (uxBits & BIT_1)  != 0 )          //状态变化
		{
			change_status_fun();
		}
		
		else if( (uxBits & BIT_2)  != 0 )          //RTK失去信号
		{
			osEventFlagsClear(Device_Run_status_eventHandle,BIT_23);                //不可启动
		}
		
    osDelay(1);
  }
  /* USER CODE END Device_unusual_task */
}

/* USER CODE BEGIN Header_EC600U_SEND_task */
/**
* @brief Function implementing the EC600U_SEND thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_EC600U_SEND_task */
void EC600U_SEND_task(void *argument)
{
  /* USER CODE BEGIN EC600U_SEND_task */
	osStatus_t err;
	uint16_t Send_Len;
  /* Infinite loop */
  for(;;)
  {
		err = osMessageQueueGet (uart4_send_semp_queueHandle, &Send_Len, 0, portMAX_DELAY);
		if(err == osOK)
		{
			HAL_UART_Transmit_DMA(&huart4,UART4TxData[UART4_fifo.usTxRead],Send_Len );
			if (++UART4_fifo.usTxRead >= UART4_fifo.usTxBufSize)
			{
				UART4_fifo.usTxRead = 0;
			}
			osDelay(100);
		}
		 osDelay(1);
  }
  /* USER CODE END EC600U_SEND_task */
}

/* USER CODE BEGIN Header_VCU_send_task */
/**
* @brief Function implementing the VCU_send thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_VCU_send_task */
void VCU_send_task(void *argument)
{
  /* USER CODE BEGIN VCU_send_task */
	osStatus_t err;
	CAN_Msg_t Msg;
  /* Infinite loop */
  for(;;)
  {
		err = osSemaphoreAcquire (CAN_send_sempHandle, 30);
		if(err == osOK)
		{
			/* 发送信息 */
			can_SendPacket(CAN1TxData[CAN1_fifo.usTxRead],DRIVEID);
			memset(CAN1TxData[CAN1_fifo.usTxRead],0,8);
			if (++CAN1_fifo.usTxRead >= CAN1_fifo.usTxBufSize)
			{
				CAN1_fifo.usTxRead = 0;
			}
			osDelay(8);
		}
		else
		{
			Msg = Direct_Drive_motor(0, 0);
			can_SendPacket((uint8_t *)Msg.L_Msg,DRIVEID);
			osDelay(3);
			can_SendPacket((uint8_t *)Msg.R_Msg,DRIVEID);
		}
    osDelay(1);
  }
  /* USER CODE END VCU_send_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

