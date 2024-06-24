/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.h
  * @brief   This file contains all the function prototypes for
  *          the fdcan.c file
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
#ifndef __FDCAN_H__
#define __FDCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern FDCAN_HandleTypeDef hfdcan1;

/* USER CODE BEGIN Private defines */
#define CAN1_Max_Txnum_size 15
#define CAN1_Max_Txbuf_size 8
#define CAN1_Max_Rxnum_size 15
#define CAN1_Max_Rxbuf_size 8

extern uint8_t CAN1RxData[CAN1_Max_Rxnum_size][CAN1_Max_Rxbuf_size];
extern uint8_t CAN1TxData[CAN1_Max_Txnum_size][CAN1_Max_Txbuf_size];


/* �����豸�ṹ�� */
typedef struct
{
	uint16_t usTxBufSize;		/* ���ջ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */

	__IO uint16_t usTxWrite;	/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;		/* ���ͻ�������ָ�� */
	
	__IO uint16_t usRxWrite;	/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;		/* ���ջ�������ָ�� */
}CAN_fifo_t;

extern CAN_fifo_t CAN1_fifo;
/* USER CODE END Private defines */

void MX_FDCAN1_Init(void);

/* USER CODE BEGIN Prototypes */
void CAN1_send_data_apply(int8_t* data);
void can_SendPacket(uint8_t *_DataBuf, uint32_t canTxid);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */

