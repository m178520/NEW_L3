/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
typedef enum  
{
	Poweron,
	Job_Wait    = 1500,
	Job_Working = 1501,
	Job_Pause   = 1502,
	Job_Finish  = 1503,
	Job_Return  = 1504,
	Job_Block   = 1541,
	Poweroff
}Device_Run_status_t;

typedef enum  
{
	Check_poweron,
	Check_Authen,
	Check_MQTT_APP,
	Check_RTK
}Device_Poweron_status_t;


typedef struct
{
	Device_Run_status_t Prestatus;
	Device_Run_status_t Curstatus;
	Device_Run_status_t Alterstatus;
}Change_Status_t;

extern Device_Poweron_status_t Device_Poweron_status;
extern Change_Status_t Device_Run_Status;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
#define BIT_0  	( 1 << 0 )
#define BIT_1	  ( 1 << 1 )
#define BIT_2	  ( 1 << 2 )
#define BIT_3	  ( 1 << 3 )
#define BIT_4	  ( 1 << 4 )
#define BIT_5	  ( 1 << 5 )
#define BIT_6	  ( 1 << 6 )
#define BIT_7	  ( 1 << 7 )
#define BIT_8	  ( 1 << 8 )
#define BIT_9	  ( 1 << 9 )
#define BIT_10 	( 1 << 10 )
#define BIT_11  ( 1 << 11 )
#define BIT_12 	( 1 << 12 )
#define BIT_13 	( 1 << 13 )
#define BIT_14 	( 1 << 14 )
#define BIT_15 	( 1 << 15 )
#define BIT_16 	( 1 << 16 )
#define BIT_17 	( 1 << 17 )
#define BIT_18 	( 1 << 18 )
#define BIT_19 	( 1 << 19 )
#define BIT_20 	( 1 << 20 )
#define BIT_21 	( 1 << 21 )
#define BIT_22 	( 1 << 22 )
#define BIT_23 	( 1 << 23 )
// extern const uint32_t BIT_0;
// extern const uint32_t BIT_1;
// extern const uint32_t BIT_2;
// extern const uint32_t BIT_3;
// extern const uint32_t BIT_4;
// extern const uint32_t BIT_5;
// extern const uint32_t BIT_6;
// extern const uint32_t BIT_7;
// extern const uint32_t BIT_8;
// extern const uint32_t BIT_9;
// extern const uint32_t BIT_10;
// extern const uint32_t BIT_11;
// extern const uint32_t BIT_12;
// extern const uint32_t BIT_13;
// extern const uint32_t BIT_14;
// extern const uint32_t BIT_15;
// extern const uint32_t BIT_16;
// extern const uint32_t BIT_17;
// extern const uint32_t BIT_18;
// extern const uint32_t BIT_19;
// extern const uint32_t BIT_20;
// extern const uint32_t BIT_21;
// extern const uint32_t BIT_22;
// extern const uint32_t BIT_23;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
