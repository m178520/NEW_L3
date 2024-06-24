/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart5;

/* USER CODE BEGIN Private defines */
#define UART4_Max_Rxnum_size 15
#define UART4_Max_Txnum_size 15
#define UART4_Max_Rxbuf_size 700
#define UART4_Max_Txbuf_size 1500

extern uint8_t UART4RxData[UART4_Max_Rxnum_size][UART4_Max_Rxbuf_size];
extern uint8_t UART4TxData[UART4_Max_Txnum_size][UART4_Max_Txbuf_size];

#define UART5_Max_Rxnum_size 30
#define UART5_Max_Rxbuf_size 40

extern uint8_t UART5RxData[UART5_Max_Rxnum_size][UART5_Max_Rxbuf_size];
/* USER CODE END Private defines */

void MX_UART4_Init(void);
void MX_UART5_Init(void);

/* USER CODE BEGIN Prototypes */
/* 串口设备结构体 */
typedef struct
{
	uint16_t usTxBufSize;		/* 接收缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */

	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	
	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
}UART_fifo_t;

extern UART_fifo_t UART4_fifo;
extern UART_fifo_t UART5_fifo;
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

