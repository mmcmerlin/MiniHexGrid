/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "neopixel_driver.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>
#include "main.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim3;
int16_t encoderPosition = 0;
int16_t lastEncoderPosition = 0;
uint8_t Index = 0;
/* USER CODE END Variables */
/* Definitions for UARTTask */
osThreadId_t UARTTaskHandle;
const osThreadAttr_t UARTTask_attributes = {
  .name = "UARTTask",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 128 * 4
};
/* Definitions for NeoPixelTask */
osThreadId_t NeoPixelTaskHandle;
const osThreadAttr_t NeoPixelTask_attributes = {
  .name = "NeoPixelTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for EncoderTask */
osThreadId_t EncoderTaskHandle;
const osThreadAttr_t EncoderTask_attributes = {
  .name = "EncoderTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 128 * 4
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .priority = (osPriority_t) osPriorityAboveNormal1,
  .stack_size = 128 * 4
};
/* Definitions for ServoTask */
osThreadId_t ServoTaskHandle;
const osThreadAttr_t ServoTask_attributes = {
  .name = "ServoTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for displayMutex */
osMutexId_t displayMutexHandle;
const osMutexAttr_t displayMutex_attributes = {
  .name = "displayMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	ssd1306_Init();
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  /* USER CODE END Init */
  /* creation of displayMutex */
  displayMutexHandle = osMutexNew(&displayMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of UARTTask */
  UARTTaskHandle = osThreadNew(StartUARTTask, NULL, &UARTTask_attributes);

  /* creation of NeoPixelTask */
  NeoPixelTaskHandle = osThreadNew(StartNeoPixelTask, NULL, &NeoPixelTask_attributes);

  /* creation of EncoderTask */
  EncoderTaskHandle = osThreadNew(StartEncoderTask, NULL, &EncoderTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

  /* creation of ServoTask */
  ServoTaskHandle = osThreadNew(StartServoTask, NULL, &ServoTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartUARTTask */
/**
* @brief Function implementing the UARTTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUARTTask */
void StartUARTTask(void *argument)
{
  /* USER CODE BEGIN UARTTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END UARTTask */
}

/* USER CODE BEGIN Header_StartNeoPixelTask */
/**
* @brief Function implementing the NeoPixelTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartNeoPixelTask */
void StartNeoPixelTask(void *argument)
{
  /* USER CODE BEGIN NeoPixelTask */
  NEOPIXEL_Init();
  /* Infinite loop */
  for(;;)
  {
	NEOPIXEL_SetColour(0, 255, 255, 255);
	NEOPIXEL_SetColour(1, 0, 0, 0);
	NEOPIXEL_SetColour(2, 255, 255, 255);
	NEOPIXEL_SetColour(3, 0, 0, 0);
	NEOPIXEL_Update();
	osDelay(1000);
	NEOPIXEL_SetColour(0, 0, 0, 0);
	NEOPIXEL_SetColour(1, 255, 255, 255);
	NEOPIXEL_SetColour(2, 0, 0, 0);
	NEOPIXEL_SetColour(3, 255, 255, 255);
	NEOPIXEL_Update();
	osDelay(1000);
  }
  /* USER CODE END NeoPixelTask */
}

/* USER CODE BEGIN Header_StartEncoderTask */
/**
* @brief Function implementing the EncoderTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartEncoderTask */
void StartEncoderTask(void *argument)
{
  /* USER CODE BEGIN EncoderTask */
  /* Infinite loop */
  for(;;)
  {
		encoderPosition = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);

		if (encoderPosition != lastEncoderPosition) {
		  if (encoderPosition > lastEncoderPosition) {
			  Index++;
		  }
		  lastEncoderPosition = encoderPosition;
		}
	    osDelay(10);
  }
  /* USER CODE END EncoderTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN DisplayTask */
  /* Infinite loop */
  for(;;)
  {
	ssd1306_Fill(White);
    ssd1306_UpdateScreen();
    osDelay(1000);
	ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    osDelay(1000);
  }
  /* USER CODE END DisplayTask */
}

/* USER CODE BEGIN Header_StartServoTask */
/**
* @brief Function implementing the ServoTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartServoTask */
void StartServoTask(void *argument)
{
  /* USER CODE BEGIN ServoTask */
  /* Infinite loop */
  for(;;)
  {
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4, 1000);   // Move to -90° ~1ms pulse
	osDelay(1000);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4, 1500);// Move to 0° ~1.5ms pulse
	osDelay(1000);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4, 2000); // Move to 90° ~2ms pulse
	osDelay(1000);
  }
  /* USER CODE END ServoTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

