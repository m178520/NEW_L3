/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */
#define SPI1_Max_Rxnum_size 15
#define SPI1_Max_Rxbuf_size 200

#define SPI1_Max_Txnum_size 15
#define SPI1_Max_Txbuf_size 512

/* �����豸�ṹ�� */
typedef struct
{
	uint16_t usTxBufSize;		/* ���ջ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */

	__IO uint16_t usTxWrite;	/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;		/* ���ͻ�������ָ�� */
	
	__IO uint16_t usRxWrite;	/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;		/* ���ջ�������ָ�� */
}SPI1_fifo_t;

//extern uint8_t SPI1TxData[SPI1_Max_Txnum_size][SPI1_Max_Txbuf_size];
extern uint8_t SPI1RxData[SPI1_Max_Rxnum_size][SPI1_Max_Rxbuf_size];
extern SPI1_fifo_t SPI1_fifo;
/* USER CODE END Private defines */

void MX_SPI1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

