/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.h
  * Description        : FreeRTOS applicative header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef __APP_FREERTOS_H__
#define __APP_FREERTOS_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Exported macro -------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */
extern osThreadId_t NeoPixelTaskHandle;
extern osThreadId_t EncoderTaskHandle;
extern osThreadId_t DisplayTaskHandle;
extern osThreadId_t ServoTaskHandle;
extern osMutexId_t displayMutexHandle;
extern osThreadId_t UARTTaskHandle;
extern osMessageQueueId_t UARTMailQueueHandle;

/* Exported function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartNeoPixelTask(void *argument);
void StartEncoderTask(void *argument);
void StartDisplayTask(void *argument);
void StartServoTask(void *argument);
void StartUARTTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

#ifdef __cplusplus
}
#endif
#endif /* __APP_FREERTOS_H__ */
