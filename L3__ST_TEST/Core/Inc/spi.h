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

/* 串口设备结构体 */
typedef struct
{
	uint16_t usTxBufSize;		/* 接收缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */

	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	
	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
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

