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
	CAN1_Max_Txnum_size,		  /* ���ͻ�������С */
	CAN1_Max_Rxnum_size,      /* ���ջ�������С */

	0,	/* ���ͻ�����дָ�� */
	0,	/* ���ͻ�������ָ�� */
	
	0,	/* ���ջ�����дָ�� */
	0,	/* ���ջ�������ָ�� */
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
		���ù�����, ��������Ҫ���ڽ��գ���������λģʽ��
		FilterID1 = filter
		FilterID2 = mask
		
		FilterID2��maskÿ��bit����
		0: �����ģ���λ�����ڱȽϣ�
		1: ����ƥ�䣬���յ���ID�������˲�����Ӧ��IDλ��һ�¡�
		
		����˵����
		FilterID1 = 0x111  001 0001 0001 
		FilterID2 = 0x7FF  111 1111 1111
		��ʾ������IDΪ0x111��FDCAN֡��
	*/
	FDCAN_FilterTypeDef sFilterConfig1;
	sFilterConfig1.IdType = FDCAN_STANDARD_ID;              /* ���ñ�׼ID������չID */
	sFilterConfig1.FilterIndex = 0;   						          /* ���ڹ�����������׼ID����Χ0��127 */
	sFilterConfig1.FilterType = FDCAN_FILTER_MASK;          /* ��������������λģʽ -----------*/
	sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;  /* �������ƥ�䣬�����ݱ��浽Rx FIFO 0 */
	sFilterConfig1.FilterID1 = 0x601;                       /* ����λģʽ�£�FilterID1����ϢID */
	sFilterConfig1.FilterID2 = 0x000; 						          /* ����λģʽ�£�FilterID2����Ϣ����λ */
	HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig1);      /* ���ù����� */
	
		HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,                 /* ȫ�ֹ������� */
	                              FDCAN_REJECT,             /* ���յ���ϢID���׼ID���˲�ƥ�䣬������ */
																FDCAN_REJECT,             /* ���յ���ϢID����չID���˲�ƥ�䣬������ */
																FDCAN_FILTER_REMOTE,      /* ���˱�׼IDԶ��֡ */ 
																FDCAN_FILTER_REMOTE);     /* ������չIDԶ��֡ */ 
									
//		/* ����Rx FIFO0����ֵΪ1 ---------------------------------------*/
//	HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 1);

//	/* ����RX FIFO0����ֵ�ж� */
//	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_WATERMARK, 0);
	
	/* ��6����Rx FIFO1�ж����� ===================================================================*/	
	/* ����RX FIFO1���������ж� */
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0); //8λ�����ж�
	
//	
//	/* ��7����Rx Buffer�ж����� ==================================================================*/	
//	/* Rx Buffer���յ�����Ϣ�ʹ����ж� */
//	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_BUFFER_NEW_MESSAGE, 0);	
	
	/* ����FDCAN */
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
*	�� �� ��: can_SendPacket
*	����˵��: ����һ������
*	��    �Σ�_DataBuf ���ݻ�����
*			  _Len ���ݳ���
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
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void can_SendPacket(uint8_t *_DataBuf, uint32_t canTxid)
{		
	FDCAN_TxHeaderTypeDef TxHeader = {0};

	/* ��1�������÷��Ͳ��� ===================================================================*/
	TxHeader.Identifier = canTxid;             		         /* ���ý���֡��Ϣ��ID */
	TxHeader.IdType = FDCAN_STANDARD_ID;     		         /* ��׼ID */
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;		         /* ����֡ */
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;      			 /* �������ݳ��� */
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;     /* ���ô���״ָ̬ʾ */
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;              /* �����ɱ䲨���� */
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;               /* FDCAN��ʽ */
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;    /* ���ڷ����¼�FIFO����, ���洢 */
	TxHeader.MessageMarker = 1;                          /* ���ڸ��Ƶ�TX EVENT FIFO����ϢMaker��ʶ����Ϣ״̬����Χ0��0xFF */
	
	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, _DataBuf); /* ������Ҫ�������޸�_DataBuf��ֵ */
}
/*
CAN1�ж���0������Ϣ�����жϻص�����
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
			/* ��RX FIFO0��ȡ���� */
			HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &g_Can1RxHeader, CAN1Rxbuff);
//			if(strlen(CAN1Rxbuff) > 1)
//			{
//				printf("%d     \r\n",strlen(CAN1Rxbuff));
//				memcpy(USART1TxData[UART_fifo.usTxWrite],CAN1Rxbuff,strlen(CAN1Rxbuff));
//				printf("%s\r\n",USART1TxData[UART_fifo.usTxWrite]);
//			}

			/* ����ʹ��RX FIFO0��ֵ�ж� */
			HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
		}
}
/* USER CODE END 1 */
