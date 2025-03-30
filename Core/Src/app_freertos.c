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
#include <main.h>
#include <string.h>
#include <stdlib.h>

#include <sim.h>
#include <display_driver.h>
#include <servo_routine.h>

#include <neopixel_driver.h>
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
extern RNG_HandleTypeDef hrng;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim3;

extern uint8_t currentIndex;
extern Menu *currentMenu;
extern uint8_t startIndex;

extern int *adjustingValue;
extern int adjustMin, adjustMax;

MODULE module;

// Encoder variables
int16_t encoderPosition = 0;
int16_t lastEncoderPosition = 0;
uint8_t buttonPressed = 0;

int16_t realPower, reactivePower;
int totalLoad;
int transformerStatus;
int hostFreq, hostPower;

//servo variables
int8_t servo_speed;

/* USER CODE END Variables */
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
/* Definitions for UARTTask */
osThreadId_t UARTTaskHandle;
const osThreadAttr_t UARTTask_attributes = {
  .name = "UARTTask",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 128 * 4
};
/* Definitions for ButtonTask */
osThreadId_t ButtonTaskHandle;
const osThreadAttr_t ButtonTask_attributes = {
  .name = "ButtonTask",
  .priority = (osPriority_t) osPriorityAboveNormal2,
  .stack_size = 128 * 4
};
/* Definitions for GameTask */
osThreadId_t GameTaskHandle;
const osThreadAttr_t GameTask_attributes = {
  .name = "GameTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for ValueTask */
osThreadId_t ValueTaskHandle;
const osThreadAttr_t ValueTask_attributes = {
  .name = "ValueTask",
  .priority = (osPriority_t) osPriorityAboveNormal7,
  .stack_size = 128 * 4
};
/* Definitions for displayMutex */
osMutexId_t displayMutexHandle;
const osMutexAttr_t displayMutex_attributes = {
  .name = "displayMutex"
};
/* Definitions for ReceiveQueue */
osMessageQueueId_t ReceiveQueueHandle;
const osMessageQueueAttr_t ReceiveQueue_attributes = {
  .name = "ReceiveQueue"
};
/* Definitions for TransmitQueue1 */
osMessageQueueId_t TransmitQueue1Handle;
const osMessageQueueAttr_t TransmitQueue1_attributes = {
  .name = "TransmitQueue1"
};
/* Definitions for TransmitQueue2 */
osMessageQueueId_t TransmitQueue2Handle;
const osMessageQueueAttr_t TransmitQueue2_attributes = {
  .name = "TransmitQueue2"
};
/* Definitions for TransmitQueue3 */
osMessageQueueId_t TransmitQueue3Handle;
const osMessageQueueAttr_t TransmitQueue3_attributes = {
  .name = "TransmitQueue3"
};
/* Definitions for AdjustSem */
osSemaphoreId_t AdjustSemHandle;
const osSemaphoreAttr_t AdjustSem_attributes = {
  .name = "AdjustSem"
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

  /* USER CODE END Init */
  /* creation of displayMutex */
  displayMutexHandle = osMutexNew(&displayMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */
  /* creation of AdjustSem */
  AdjustSemHandle = osSemaphoreNew(1, 1, &AdjustSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */
  /* creation of ReceiveQueue */
  ReceiveQueueHandle = osMessageQueueNew (24, sizeof(MSG_EVENT), &ReceiveQueue_attributes);
  /* creation of TransmitQueue1 */
  TransmitQueue1Handle = osMessageQueueNew (8, sizeof(MSG_EVENT), &TransmitQueue1_attributes);
  /* creation of TransmitQueue2 */
  TransmitQueue2Handle = osMessageQueueNew (8, sizeof(MSG_EVENT), &TransmitQueue2_attributes);
  /* creation of TransmitQueue3 */
  TransmitQueue3Handle = osMessageQueueNew (8, sizeof(MSG_EVENT), &TransmitQueue3_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of EncoderTask */
  EncoderTaskHandle = osThreadNew(StartEncoderTask, NULL, &EncoderTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

  /* creation of UARTTask */
  UARTTaskHandle = osThreadNew(StartUARTTask, NULL, &UARTTask_attributes);

  /* creation of ButtonTask */
  ButtonTaskHandle = osThreadNew(StartButtonTask, NULL, &ButtonTask_attributes);

  /* creation of GameTask */
  GameTaskHandle = osThreadNew(StartGameTask, NULL, &GameTask_attributes);

  /* creation of ValueTask */
  ValueTaskHandle = osThreadNew(StartValueTask, NULL, &ValueTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  /* Infinite loop */
  for(;;)
  {
	    // Skip if adjusting
	    if (currentMenu == NULL || currentMenu->adjustFunc != NULL) {
	        osDelay(10);
	        continue;
	    }

	    // CRITICAL GUARD: skip if itemCount is invalid
	    if (currentMenu->itemCount == 0 || currentMenu->items == NULL) {
	        osDelay(10);
	        continue;
	    }

	    int delta = getEncoderDelta();

	    if (abs(delta) >= 2) {  // Threshold: ignore minor noise
	        if (delta > 0) {
	            currentIndex = (currentIndex + 1) % currentMenu->itemCount;
	            if (currentIndex >= startIndex + WINDOW_SIZE) {
	                startIndex++;
	            }
	        } else {
	            currentIndex = (currentIndex == 0) ? currentMenu->itemCount - 1 : currentIndex - 1;
	            if (currentIndex < startIndex && startIndex > 0) {
	                startIndex--;
	            }
	        }

	        // Reset encoder position to avoid drift
	        lastEncoderPosition = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
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
  ssd1306_Init();
  /* Infinite loop */
  for(;;)
  {
	if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) {
		updateMenuDisplay();
		osMutexRelease(displayMutexHandle);
	}
	osDelay(40);
    // If the menu has a ShowInfo function, refresh frequently
    if (currentMenu->showInfo != NULL) {
    	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET) {
    	    navigateBack();
    	    osDelay(200); // debounce
    	    continue;
    	}
    	realPower = self.local.bus.real/10;
    	reactivePower = self.local.bus.reactive/10;
    }
    static uint8_t adjustTriggered = 0;
    if (currentMenu->adjustFunc != NULL && !adjustTriggered) {
        osSemaphoreRelease(AdjustSemHandle);
        adjustTriggered = 1;
    } else if (currentMenu->adjustFunc == NULL) {
        adjustTriggered = 0;  // Reset when not in adjust mode
    }
    else {
        osDelay(40); // Normal refresh rate
    }
  }
  /* USER CODE END DisplayTask */
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
	SIM_Init(&module);

	/* Infinite loop */
	for (;;) {
		SIM_Loop();
	}
  /* USER CODE END UARTTask */
}

/* USER CODE BEGIN Header_StartButtonTask */
/**
* @brief Function implementing the ButtonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartButtonTask */
void StartButtonTask(void *argument)
{
  /* USER CODE BEGIN ButtonTask */
	/*Establish parent menus*/
	setupMenus();
  /* Infinite loop */
  for(;;)
  {
	if (currentMenu->adjustFunc != NULL) {
	  osDelay(50); // we're in adjustment mode — let ValueTask handle the button
	  continue;
	}

	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET && !buttonPressed)
	{
	    buttonPressed = 1;
	    if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) {
	        Menu *previousMenu = currentMenu;  // Save current before selection
	        handleSelection();                 // Might change currentMenu
	        osMutexRelease(displayMutexHandle);

	        // Check if we navigated into a new menu (submenu)
	        if (currentMenu != previousMenu && currentMenu->adjustFunc == NULL) {
	            lastEncoderPosition = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
	        }

	        osDelay(50);
	    }
	}
	else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET)
	{
	  buttonPressed = 0;
	}
    osDelay(50);
  }
  /* USER CODE END ButtonTask */
}

/* USER CODE BEGIN Header_StartGameTask */
/**
* @brief Function implementing the GameTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGameTask */
void StartGameTask(void *argument)
{
  /* USER CODE BEGIN GameTask */
  /* Infinite loop */
	/* USER CODE BEGIN GameTask */
	if (self.module == SIM_MASTER) {
		// clear all board addresses on init
		SIM_MESSAGE clear;
		clear.meta.type = SIM_CLEAR;
		SIM_Transmit(SIM_MASTER_PORT, clear);

		self.game.time = 0;
		self.game.frequency = 0;

		SIM_MESSAGE update;
		update.meta.type = SIM_UPDATE;
		update.meta.path = 0;
		update.meta.location = 0;
		for(;;) {
			// clear rx buffer
			ports[SIM_MASTER_PORT].rx_ctr = 0;
			for (uint8_t msg = 0; msg < 16; msg++) {
				for (uint8_t byte = 0; byte < SIM_MSG_LEN; byte++) {
					ports[SIM_MASTER_PORT].rx_buf[msg].buffer[byte] = 0;
				}
			}

			// send request
			self.game.time++;
			update.data.update = self.game;
			SIM_Transmit(SIM_MASTER_PORT, update);
			osDelay(500);

			for (uint8_t loc = 0; loc < 16; loc++) {
				SIM_MESSAGE tile = ports[SIM_MASTER_PORT].rx_buf[loc];
			}

			// do game
		}
	} else {
		for(;;) {
			osDelay(1);
		}
	}
  /* USER CODE END GameTask */
}

/* USER CODE BEGIN Header_StartValueTask */
/**
* @brief Function implementing the ValueTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartValueTask */
void StartValueTask(void *argument)
{
  /* USER CODE BEGIN ValueTask */
  /* Infinite loop */
  for(;;)
  {
      osSemaphoreAcquire(AdjustSemHandle, osWaitForever);

       // Capture initial encoder baseline
       lastEncoderPosition = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);

       while (1)
       {
           encoderPosition = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);

           if (adjustingValue != NULL)
           {
               if (encoderPosition < lastEncoderPosition)
               {
                   *adjustingValue = (*adjustingValue < adjustMax) ? *adjustingValue + 1 : adjustMax;
               }
               else if (encoderPosition > lastEncoderPosition)
               {
                   *adjustingValue = (*adjustingValue > adjustMin) ? *adjustingValue - 1 : adjustMin;
               }
           }

           lastEncoderPosition = encoderPosition;

           // Use updateMenuDisplay, but only if still in adjustment mode
           if (currentMenu && currentMenu->adjustFunc != NULL && adjustingValue != NULL)
           {
               if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK)
               {
                   updateMenuDisplay();  // Triggers adjustFunc + updateAdjustDisplay()
                   osMutexRelease(displayMutexHandle);
               }
           }

           // Button press to exit adjustment
           if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
           {
               exitAdjustmentMode();

               // Reset encoder state
               encoderPosition = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
               lastEncoderPosition = encoderPosition;
               currentIndex = 0;
               startIndex = 0;

               osDelay(150);  // Debounce
               break;
           }

           osDelay(40);
       }
  }
  /* USER CODE END ValueTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
int getEncoderDelta() {
    int16_t current = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
    int16_t delta = current - lastEncoderPosition;

    // Handle overflow wrap-around
    if (delta > 32767) delta -= 65536;
    if (delta < -32768) delta += 65536;

    return delta;
}
/* USER CODE END Application */

