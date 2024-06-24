/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.c
  * @brief   This file provides code for the configuration
  *          of the FDCAN instances.
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
#include "fdcan.h"

/* USER CODE BEGIN 0 */
#include "cmsis_os.h"

#include "string.h"

uint8_t CAN1RxData[CAN1_Max_Rxnum_size][CAN1_Max_Rxbuf_size] = {0};
uint8_t CAN1TxData[CAN1_Max_Txnum_size][CAN1_Max_Txbuf_size] = {0};

CAN_fifo_t CAN1_fifo = {
	CAN1_Max_Txnum_size,		  /* 发送缓冲区大小 */
	CAN1_Max_Rxnum_size,      /* 接收缓冲区大小 */

	0,	/* 发送缓冲区写指针 */
	0,	/* 发送缓冲区读指针 */
	
	0,	/* 接收缓冲区写指针 */
	0,	/* 接收缓冲区读指针 */
};

extern osSemaphoreId_t CAN_send_sempHandle;
/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = ENABLE;
  hfdcan1.Init.NominalPrescaler = 10;
  hfdcan1.Init.NominalSyncJumpWidth = 8;
  hfdcan1.Init.NominalTimeSeg1 = 31;
  hfdcan1.Init.NominalTimeSeg2 = 8;
  hfdcan1.Init.DataPrescaler = 10;
  hfdcan1.Init.DataSyncJumpWidth = 8;
  hfdcan1.Init.DataTimeSeg1 = 31;
  hfdcan1.Init.DataTimeSeg2 = 8;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.RxFifo0ElmtsNbr = 1;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.TxEventsNbr = 16;
  hfdcan1.Init.TxBuffersNbr = 1;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 1;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */
	/* 
		配置过滤器, 过滤器主要用于接收，比如屏蔽位模式。
		FilterID1 = filter
		FilterID2 = mask
		
		FilterID2的mask每个bit含义
		0: 不关心，该位不用于比较；
		1: 必须匹配，接收到的ID必须与滤波器对应的ID位相一致。
		
		举例说明：
		FilterID1 = 0x111  001 0001 0001 
		FilterID2 = 0x7FF  111 1111 1111
		表示仅接收ID为0x111的FDCAN帧。
	*/
	FDCAN_FilterTypeDef sFilterConfig1;
	sFilterConfig1.IdType = FDCAN_STANDARD_ID;              /* 设置标准ID或者扩展ID */
	sFilterConfig1.FilterIndex = 0;   						          /* 用于过滤索引，标准ID，范围0到127 */
	sFilterConfig1.FilterType = FDCAN_FILTER_MASK;          /* 过滤器采样屏蔽位模式 -----------*/
	sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;  /* 如果过滤匹配，将数据保存到Rx FIFO 0 */
	sFilterConfig1.FilterID1 = 0x601;                       /* 屏蔽位模式下，FilterID1是消息ID */
	sFilterConfig1.FilterID2 = 0x000; 						          /* 屏蔽位模式下，FilterID2是消息屏蔽位 */
	HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig1);      /* 配置过滤器 */
	
		HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,                 /* 全局过滤设置 */
	                              FDCAN_REJECT,             /* 接收到消息ID与标准ID过滤不匹配，不接受 */
																FDCAN_REJECT,             /* 接收到消息ID与扩展ID过滤不匹配，不接受 */
																FDCAN_FILTER_REMOTE,      /* 过滤标准ID远程帧 */ 
																FDCAN_FILTER_REMOTE);     /* 过滤扩展ID远程帧 */ 
									
//		/* 设置Rx FIFO0的阈值为1 ---------------------------------------*/
//	HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 1);

//	/* 开启RX FIFO0的阈值中断 */
//	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_WATERMARK, 0);
	
	/* 第6步：Rx FIFO1中断配置 ===================================================================*/	
	/* 开启RX FIFO1的新数据中断 */
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0); //8位数据中断
	
//	
//	/* 第7步：Rx Buffer中断配置 ==================================================================*/	
//	/* Rx Buffer接收到新消息就触发中断 */
//	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_BUFFER_NEW_MESSAGE, 0);	
	
	/* 启动FDCAN */
	HAL_FDCAN_Start(&hfdcan1);
  /* USER CODE END FDCAN1_Init 2 */

}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PD0     ------> FDCAN1_RX
    PD1     ------> FDCAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PD0     ------> FDCAN1_RX
    PD1     ------> FDCAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void CAN1_send_data_apply(int8_t* data)
{
	memcpy(CAN1TxData[CAN1_fifo.usTxWrite],data,8);
	if (++CAN1_fifo.usTxWrite >= CAN1_fifo.usTxBufSize)
	{
		CAN1_fifo.usTxWrite = 0;
	}
	if (osSemaphoreGetCount(CAN_send_sempHandle) < CAN1_fifo.usTxBufSize)
	{
		osSemaphoreRelease (CAN_send_sempHandle);
	}
}

/*
*********************************************************************************************************
*	函 数 名: can_SendPacket
*	功能说明: 发送一包数据
*	形    参：_DataBuf 数据缓冲区
*			  _Len 数据长度
*             FDCAN_DLC_BYTES_0  
*             FDCAN_DLC_BYTES_1  
*             FDCAN_DLC_BYTES_2   
*             FDCAN_DLC_BYTES_3   
*             FDCAN_DLC_BYTES_4 
*             FDCAN_DLC_BYTES_5 
*             FDCAN_DLC_BYTES_6 
*             FDCAN_DLC_BYTES_7 
*             FDCAN_DLC_BYTES_8 
*             FDCAN_DLC_BYTES_12
*             FDCAN_DLC_BYTES_16 
*             FDCAN_DLC_BYTES_20 
*             FDCAN_DLC_BYTES_24
*             FDCAN_DLC_BYTES_32 
*             FDCAN_DLC_BYTES_48
*             FDCAN_DLC_BYTES_64 
*	返 回 值: 无
*********************************************************************************************************
*/
void can_SendPacket(uint8_t *_DataBuf, uint32_t canTxid)
{		
	FDCAN_TxHeaderTypeDef TxHeader = {0};

	/* 第1步：配置发送参数 ===================================================================*/
	TxHeader.Identifier = canTxid;             		         /* 设置接收帧消息的ID */
	TxHeader.IdType = FDCAN_STANDARD_ID;     		         /* 标准ID */
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;		         /* 数据帧 */
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;      			 /* 发送数据长度 */
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;     /* 设置错误状态指示 */
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;              /* 开启可变波特率 */
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;               /* FDCAN格式 */
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;    /* 用于发送事件FIFO控制, 不存储 */
	TxHeader.MessageMarker = 1;                          /* 用于复制到TX EVENT FIFO的消息Maker来识别消息状态，范围0到0xFF */
	
	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, _DataBuf); /* 根据需要，可以修改_DataBuf数值 */
}
/*
CAN1中断线0的新消息接受中断回调函数
*/
FDCAN_RxHeaderTypeDef g_Can1RxHeader;
uint8_t CAN1Rxbuff[512] = {0};


void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  /* NOTE : his function Should not be modified, when the callback is needed,
            the HAL_FDCAN_RxFifo0Callback could be implemented in the user file
   */
if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
		{
			/* 从RX FIFO0读取数据 */
			HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &g_Can1RxHeader, CAN1Rxbuff);
//			if(strlen(CAN1Rxbuff) > 1)
//			{
//				printf("%d     \r\n",strlen(CAN1Rxbuff));
//				memcpy(USART1TxData[UART_fifo.usTxWrite],CAN1Rxbuff,strlen(CAN1Rxbuff));
//				printf("%s\r\n",USART1TxData[UART_fifo.usTxWrite]);
//			}

			/* 重新使能RX FIFO0阈值中断 */
			HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
		}
}
/* USER CODE END 1 */
