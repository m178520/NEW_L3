/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "string.h"

#include "cmsis_os.h"

/*串口一参数*/

uint8_t UART4RxData[UART4_Max_Rxnum_size][UART4_Max_Rxbuf_size] ={0};
uint8_t UART4TxData[UART4_Max_Txnum_size][UART4_Max_Txbuf_size] ={0};

UART_fifo_t UART4_fifo = {
	UART4_Max_Txnum_size,		  /* 接收缓冲区大小 */
	UART4_Max_Rxnum_size,     /*发送缓冲区大小*/

	0,	/* 发送缓冲区写指针 */
	0,	/* 发送缓冲区读指针 */
	
	0,	/* 接收缓冲区写指针 */
	0,	/* 接收缓冲区读指针 */
};

extern osMessageQueueId_t uart4_recv_semp_queueHandle;

/*串口三参数*/
uint8_t UART5RxData[UART5_Max_Rxnum_size][UART5_Max_Rxbuf_size] = {0};

UART_fifo_t UART5_fifo = {
	0,		  /* 接收缓冲区大小 */
	UART5_Max_Rxnum_size,     /*发送缓冲区大小*/

	0,	/* 发送缓冲区写指针 */
	0,	/* 发送缓冲区读指针 */
	
	0,	/* 接收缓冲区写指针 */
	0,	/* 接收缓冲区读指针 */
};

extern osMessageQueueId_t uart5_recv_semp_queueHandle;
/* USER CODE END 0 */

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
DMA_HandleTypeDef hdma_uart5_rx;

/* UART4 init function */
void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 460800;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}
/* UART5 init function */
void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 100000;
  huart5.Init.WordLength = UART_WORDLENGTH_9B;
  huart5.Init.StopBits = UART_STOPBITS_2;
  huart5.Init.Parity = UART_PARITY_EVEN;
  huart5.Init.Mode = UART_MODE_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXINVERT_INIT;
  huart5.AdvancedInit.RxPinLevelInvert = UART_ADVFEATURE_RXINV_ENABLE;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */
  /* USER CODE END UART5_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PA0     ------> UART4_TX
    PA1     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART4 DMA Init */
    /* UART4_RX Init */
    hdma_uart4_rx.Instance = DMA1_Stream0;
    hdma_uart4_rx.Init.Request = DMA_REQUEST_UART4_RX;
    hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.Mode = DMA_NORMAL;
    hdma_uart4_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_uart4_rx);

    /* UART4_TX Init */
    hdma_uart4_tx.Instance = DMA1_Stream1;
    hdma_uart4_tx.Init.Request = DMA_REQUEST_UART4_TX;
    hdma_uart4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_uart4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_tx.Init.Mode = DMA_NORMAL;
    hdma_uart4_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_uart4_tx);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspInit 0 */

  /* USER CODE END UART5_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART5;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART5 clock enable */
    __HAL_RCC_UART5_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**UART5 GPIO Configuration
    PB5     ------> UART5_RX
    PB6     ------> UART5_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_UART5;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* UART5 DMA Init */
    /* UART5_RX Init */
    hdma_uart5_rx.Instance = DMA1_Stream2;
    hdma_uart5_rx.Init.Request = DMA_REQUEST_UART5_RX;
    hdma_uart5_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart5_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart5_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart5_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart5_rx.Init.Mode = DMA_NORMAL;
    hdma_uart5_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart5_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart5_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_uart5_rx);

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(UART5_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspInit 1 */

  /* USER CODE END UART5_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PA0     ------> UART4_TX
    PA1     ------> UART4_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);

    /* UART4 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspDeInit 0 */

  /* USER CODE END UART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART5_CLK_DISABLE();

    /**UART5 GPIO Configuration
    PB5     ------> UART5_RX
    PB6     ------> UART5_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5|GPIO_PIN_6);

    /* UART5 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* UART5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspDeInit 1 */

  /* USER CODE END UART5_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/*串口DMA传输完成中断，本不应该发生，收满了还未发生空闲中断，所以是错误处理*/
void UART4_IDLE_DMA_RXCplt_Err_Callback(uint16_t len)
{
	/*直接将这收满的放到任务中去处理就好*/
	if (++UART4_fifo.usRxWrite >= UART4_fifo.usRxBufSize)
	{
		UART4_fifo.usRxWrite = 0;
	}
	if (osMessageQueueGetCount(uart4_recv_semp_queueHandle) < UART4_fifo.usRxBufSize)
	{
		osMessageQueuePut(uart4_recv_semp_queueHandle,&len,0,0);
	}
	HAL_UARTEx_ReceiveToIdle_DMA(&huart4, UART4RxData[UART4_fifo.usRxWrite],UART4_Max_Rxbuf_size);
}

void UART5_IDLE_DMA_RXCplt_Err_Callback(uint16_t Size)
{
	if (++UART5_fifo.usRxWrite >= UART5_fifo.usRxBufSize)
	{
		UART5_fifo.usRxWrite = 0;
	}
	if (osMessageQueueGetCount (uart5_recv_semp_queueHandle) < UART5_fifo.usRxBufSize)
	{
		osMessageQueuePut(uart5_recv_semp_queueHandle,&Size,0,0);
	}
	HAL_UARTEx_ReceiveToIdle_DMA(&huart5, UART5RxData[UART5_fifo.usRxWrite],UART5_Max_Rxbuf_size);
}

/*串口1空闲中断回调函数*/
uint8_t UART4_IDLE_Callback(uint16_t len)
{
	if(UART4RxData[UART4_fifo.usRxWrite][len-1] == '}' )
	{
		if (++UART4_fifo.usRxWrite >= UART4_fifo.usRxBufSize)
		{
			UART4_fifo.usRxWrite = 0;
		}
		if (osMessageQueueGetCount(uart4_recv_semp_queueHandle) < UART4_fifo.usRxBufSize)
		{
			osMessageQueuePut(uart4_recv_semp_queueHandle,&len,0,0);
		}
		HAL_UARTEx_ReceiveToIdle_DMA(&huart4, UART4RxData[UART4_fifo.usRxWrite],UART4_Max_Rxbuf_size);
		return 1;
	}
	else
	{
		if(len == UART4_Max_Rxbuf_size) 
		{
			memset(UART4RxData[UART4_fifo.usRxWrite],0,UART4_Max_Rxbuf_size);
			HAL_UARTEx_ReceiveToIdle_DMA(&huart4, UART4RxData[UART4_fifo.usRxWrite],UART4_Max_Rxbuf_size);
			return 1;
		}
		else 
		{
			HAL_UARTEx_ReceiveToIdle_DMA(&huart4, UART4RxData[UART4_fifo.usRxWrite] + len,UART4_Max_Rxbuf_size - len);
			return  0;
		}
	}
}


void UART5_IDLE_Callback(uint16_t Size)
{
	if (++UART5_fifo.usRxWrite >= UART5_fifo.usRxBufSize)
	{
		UART5_fifo.usRxWrite = 0;
	}
	if (osMessageQueueGetCount (uart5_recv_semp_queueHandle) < UART5_fifo.usRxBufSize)
	{
		osMessageQueuePut(uart5_recv_semp_queueHandle,&Size,0,0);
	}
	HAL_UARTEx_ReceiveToIdle_DMA(&huart5, UART5RxData[UART5_fifo.usRxWrite],UART5_Max_Rxbuf_size);
}
/*
UART4串口接受空闲中断回调函数
*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	static uint16_t rec_len = 0;
	/*正常串口空闲中断*/
	if(huart->RxEventType == HAL_UART_RXEVENT_IDLE)
	{
		if(huart->Instance == UART4)
		{
			rec_len += Size;
			if(UART4_IDLE_Callback(rec_len))
			{
				rec_len = 0;
			}
		}
		else if(huart->Instance == USART2)
		{
			
		}
		else if(huart->Instance == UART5)
		{
			UART5_IDLE_Callback(Size);
		}
	}
	/*DMA接收完成中断*/
	/*在串口空闲中断中如果是DMA完成中断了，却没有进行空闲中断，这时候就不能再接了，只能关闭dma重新使能*/
	else if(huart->RxEventType == HAL_UART_RXEVENT_TC)
	{
		if(huart->Instance == UART4)
		{
			UART4_IDLE_DMA_RXCplt_Err_Callback(Size);
			rec_len = 0;
		}
		else if(huart->Instance == USART2)
		{
			
		}
		else if(huart->Instance == UART5)
		{
			UART5_IDLE_DMA_RXCplt_Err_Callback(Size);
		}
	}
	/*DMA接收一半完成中断*/
	else if(huart->RxEventType == HAL_UART_RXEVENT_HT)
	{
	
	}
	
}
/*经过调试发现，sbus出现问题时，进入的回调函数为这一个，所以在这个内部写出现错误后的处理办法*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	 if(huart->Instance == UART5)
		{
			/*重新接受即可*/
			HAL_UARTEx_ReceiveToIdle_DMA(&huart5, UART5RxData[UART5_fifo.usRxWrite],UART5_Max_Rxbuf_size);
		}
	
}
///*串口错误回调函数*/
//void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
//{
//if(huart->Instance == UART4)
//{
//	uint8_t len = 0;
//	if (++UART4_fifo.usRxWrite >= UART4_fifo.usRxBufSize)
//	{
//		UART4_fifo.usRxWrite = 0;
//	}
//	if (osMessageQueueGetCount(uart4_recv_semp_queueHandle) < UART4_fifo.usRxBufSize)
//	{
//		osMessageQueuePut(uart4_recv_semp_queueHandle,&len,0,0);
//	}
//}
//}
/* USER CODE END 1 */
