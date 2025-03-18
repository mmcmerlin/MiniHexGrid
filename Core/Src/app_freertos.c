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
#include "main.h"
#include <string.h>

#include "display_driver.h"
#include "servo_routine.h"

#include "neopixel_driver.h"
#include "sim_driver.h"
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

// Encoder variables
int16_t encoderPosition = 0;
int16_t lastEncoderPosition = 0;
uint8_t buttonPressed = 0;

// Simulation variables
SIM_DATA self;
SIM_PORT ports[3] = {
		{.huart = &huart1, .tx_queue = &UARTTx1QueueHandle, .rx_ctr = 0, .tx_ctr = 0},
		{.huart = &huart2, .tx_queue = &UARTTx2QueueHandle, .rx_ctr = 0, .tx_ctr = 0},
		{.huart = &huart3, .tx_queue = &UARTTx3QueueHandle, .rx_ctr = 0, .tx_ctr = 0},
};
// Variables for Adjusting
static int *adjustingValue;
static const char *adjustLabel;
static int adjustMin, adjustMax;

//Adjusting encoder value
//int *valueToAdjust = NULL;  // Initialize as NULL
//char labelToShow[20];
//int minVal, maxVal;
////default values
//int tapChange = 100;
//int load = 100;
//int resist = 100;
//int react = 50;
//int active = 100;
//int reactive = 20;
int16_t realPower, reactivePower;
int totalLoad;
int transformerStatus;
int hostFreq, hostPower;

//servo variables
int8_t servo_speed;

/* USER CODE END Variables */
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
/* Definitions for UARTMailQueue */
osMessageQueueId_t UARTMailQueueHandle;
const osMessageQueueAttr_t UARTMailQueue_attributes = {
  .name = "UARTMailQueue"
};
/* Definitions for UARTTx1Queue */
osMessageQueueId_t UARTTx1QueueHandle;
const osMessageQueueAttr_t UARTTx1Queue_attributes = {
  .name = "UARTTx1Queue"
};
/* Definitions for UARTTx2Queue */
osMessageQueueId_t UARTTx2QueueHandle;
const osMessageQueueAttr_t UARTTx2Queue_attributes = {
  .name = "UARTTx2Queue"
};
/* Definitions for UARTTx3Queue */
osMessageQueueId_t UARTTx3QueueHandle;
const osMessageQueueAttr_t UARTTx3Queue_attributes = {
  .name = "UARTTx3Queue"
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
  /* creation of UARTMailQueue */
  UARTMailQueueHandle = osMessageQueueNew (16, sizeof(SIM_EVENT), &UARTMailQueue_attributes);
  /* creation of UARTTx1Queue */
  UARTTx1QueueHandle = osMessageQueueNew (16, sizeof(SIM_EVENT), &UARTTx1Queue_attributes);
  /* creation of UARTTx2Queue */
  UARTTx2QueueHandle = osMessageQueueNew (16, sizeof(SIM_EVENT), &UARTTx2Queue_attributes);
  /* creation of UARTTx3Queue */
  UARTTx3QueueHandle = osMessageQueueNew (16, sizeof(SIM_EVENT), &UARTTx3Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of NeoPixelTask */
  NeoPixelTaskHandle = osThreadNew(StartNeoPixelTask, NULL, &NeoPixelTask_attributes);

  /* creation of EncoderTask */
  // EncoderTaskHandle = osThreadNew(StartEncoderTask, NULL, &EncoderTask_attributes);

  /* creation of DisplayTask */
  // DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

  /* creation of ServoTask */
  // ServoTaskHandle = osThreadNew(StartServoTask, NULL, &ServoTask_attributes);

  /* creation of UARTTask */
  UARTTaskHandle = osThreadNew(StartUARTTask, NULL, &UARTTask_attributes);

  /* creation of ButtonTask */
  // ButtonTaskHandle = osThreadNew(StartButtonTask, NULL, &ButtonTask_attributes);

  /* creation of GameTask */
  GameTaskHandle = osThreadNew(StartGameTask, NULL, &GameTask_attributes);

  /* creation of ValueTask */
  // ValueTaskHandle = osThreadNew(StartValueTask, NULL, &ValueTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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
  NEOPIXEL_Update();
  /* Infinite loop */
  for(;;) {
  	osDelay(20);
		NEOPIXEL_Update();
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
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
  /* Infinite loop */
  for(;;)
  {
		encoderPosition = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);

		if (encoderPosition != lastEncoderPosition) {
		  if (encoderPosition > lastEncoderPosition) {
			currentIndex = (currentIndex + 1) % currentMenu->itemCount;
			if (currentIndex >= startIndex + WINDOW_SIZE) {
			  startIndex++;
			}
		  } else {
			currentIndex = (currentIndex == 0) ? currentMenu->itemCount - 1 : currentIndex - 1;
			if (currentIndex < startIndex) {
			  startIndex--;
			}
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
  ssd1306_Init();
  //default menu
  currentMenu = &windMenu;
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
        for (int i = 0; i < 10; i++) { // Check every 100ms x 10 = 1 second
            if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET) {
                navigateBack();
                break;
            }
            osDelay(100);
        }
    	realPower = self.local.bus.real/100;
    	reactivePower = self.local.bus.reactive/100;

    } else if (currentMenu->adjustFunc != NULL) {
        // Call the adjustment function (which now wakes the task)
        currentMenu->adjustFunc();
    } else {
        osDelay(40); // Normal refresh rate
    }
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
  Servo_Init(&htim4, TIM_CHANNEL_4);
  //HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  /* Infinite loop */
  for(;;)
  {
	servo_speed = self.other.wind.speed * (-1/2);  //speed as a percentage
	Servo_SetSpeed(servo_speed); // Rotate FS90R at according to speed of the turbine
	osDelay(1000);
  }
  /* USER CODE END ServoTask */
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
	SIM_Init();

	/* Infinite loop */
	for (;;) {
		SIM_EVENT event;
		uint8_t priority;
		// wait until a message can be retrieved from the queue
		osMessageQueueGet(UARTMailQueueHandle, &event, &priority, osWaitForever);

		// process the message
		SIM_MESSAGE copy = *event.message;
		uint8_t port = SIM_FindPort(event.huart);

		if (self.module == SIM_MASTER) {
			if (copy.meta.type == SIM_RESPONSE) {

			}
		} else {
			switch (copy.meta.type) {
			case SIM_CLEAR:
				if (self.mapped) {
					SIM_Transmit(self.left, copy);
					SIM_Transmit(self.right, copy);
					self.mapped = 0;
				}
				break;
			case SIM_REQUEST:
				// assign our ports if we haven't already
				if (!self.mapped) {
					self.path = copy.meta.path;
					self.location = copy.meta.location;

					self.upstream = port;
					self.left = (port + 1) % 3;
					self.right = (port + 2) % 3;
					self.mapped = 1;
				}

				self.game = copy.data.request;

				// reset LEDs
				NEOPIXEL_SetColour(1, 255, 191, 0);
				NEOPIXEL_SetColour(2, 255, 191, 0);
				NEOPIXEL_SetColour(3, 255, 191, 0);

				// check compatibility
				if (SIM_Compatible(self.module, copy.meta.last)) {
					self.disabled = 0;
					NEOPIXEL_SetColour(port + 1, 0, 255, 0);
				}
				else {
					self.disabled = 1;
					NEOPIXEL_SetColour(port + 1, 255, 0, 0);
				}

				// relay the message on both ports after editing the address and sender
				copy.meta.last = self.module;
				copy.meta.location++;
				SIM_Transmit(self.left, copy);
				copy.meta.path |= 0x01 << (copy.meta.location - 1);
				SIM_Transmit(self.right, copy);

				// update game state
				switch (self.module) {
					case SIM_CCGT: {
						// generation approaches setpoint
						float ratio = (float) self.other.ccgt.delta / 255;
						self.local.bus.real += (ratio * (self.other.ccgt.setpoint - self.local.bus.real));
						break;
					}
					case SIM_NUCLEAR: {
						// generation approaches setpoint
						float ratio = (float) self.other.nuclear.delta / 255;
						self.local.bus.real += (ratio * (self.other.nuclear.setpoint - self.local.bus.real));
						break;
					}
					case SIM_WIND: {
						// set new wind speed
						int16_t change = SIM_RandInt(-5, 6);
						if (self.other.wind.speed + change > 300) self.other.wind.speed = 300;
						else if (self.other.wind.speed + change < 0) self.other.wind.speed = 0;
						else self.other.wind.speed += change;

						// update generation
						if (self.other.wind.speed <= self.other.wind.cutin || self.other.wind.speed >= self.other.wind.cutout) {
							self.local.bus.real = 0;
						} else if (self.other.wind.speed >= self.other.wind.rated) {
							self.local.bus.real = self.other.wind.rating * self.other.wind.turbines;
						} else {
							float diff = (float) (self.other.wind.speed - self.other.wind.cutin) / (self.other.wind.cutout - self.other.wind.cutin);
							self.local.bus.real = diff * self.other.wind.rating * 10;
						}
						break;
					}
					case SIM_SOLAR: {
						// set new intensity
						if (self.game.time == 0) self.other.solar.intensity = SIM_RandInt(10, 255);
						else if (self.game.time == 127) self.other.solar.intensity = 0;

						// update generation
						self.local.bus.real = self.other.solar.intensity * self.other.solar.area * 10;
						break;
					}
					case SIM_CITY: {
						int16_t delta = SIM_RandInt(self.other.city.population / 100, self.other.city.population / 50);
						self.local.bus.real = 10 * self.other.city.population + (delta * (self.game.time + 64));
						break;
					}
					case SIM_FACTORY: {
						// set new intensity
						if (self.game.time == 0) self.other.solar.intensity = SIM_RandInt(10, 255);
						else if (self.game.time == 127) self.other.solar.intensity = 0;

						// update generation
						self.local.bus.real = self.other.solar.intensity * self.other.solar.area * 10;
						break;
					}
					case SIM_CARPARK: {
						// slight random variation
						self.local.bus.real = self.other.carpark.spaces * 3;
						break;
					}
					case SIM_PUMPEDHYDRO: {
						break;
					}
					case SIM_LITHIUM: {
						break;
					}
					case SIM_TRANSMISSION: {
						break;
					}
					case SIM_TRANSFORMER: {
						break;
					}
				}

				// send your local data upstream to the master
				SIM_MESSAGE response;
				response.meta.type = SIM_RESPONSE;
				response.meta.path = self.path;
				response.meta.location = self.location;
				response.meta.origin = self.module;
				response.meta.last = self.module;

				response.data.response = self.local;

				// if (!self.disabled)
				{
					SIM_Transmit(self.upstream, response);
				}
				break;
			case SIM_RESPONSE:
				// immediately relay the message to the master
				SIM_Transmit(self.upstream, copy);

				// check compatibility
				if (SIM_Compatible(self.module, copy.meta.last)) {
					NEOPIXEL_SetColour(port + 1, 0, 255, 0);
				} else {
					NEOPIXEL_SetColour(port + 1, 255, 0, 0);
				}
				break;
			case SIM_UPDATE:
				// check if we are the recipient
				if (copy.meta.location) {
					copy.meta.location--;
					uint8_t direction = copy.meta.path & 0x01;
					copy.meta.path >>= 1;

					// relay the copy on the correct port
					SIM_Transmit((self.right * direction) + (self.left * !direction), copy);
				} else {
					// copy the data for ourselves
					self.received = copy.data.update;
				}
				break;
			}
		}
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
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET && !buttonPressed)
	{
		buttonPressed = 1;
		//test
		  if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) {
			handleSelection();
			osDelay(50);
			osMutexRelease(displayMutexHandle);
		  }

//test
//		if (currentMenu == &tformerModule && strcmp(currentMenu->items[currentIndex], "Tap changer") == 0){
//			valueToAdjust = &tapChange;
//			strcpy(labelToShow, "Adjust tap value");
//			minVal = 95;
//			maxVal = 120;
//		}
//		else if (currentMenu == &cityModule && strcmp(currentMenu->items[currentIndex], "Load value") == 0){
//			valueToAdjust = &load;
//			strcpy(labelToShow, "Adjust load");
//			minVal = 50;
//			maxVal = 1000;
//		}
//		else if (currentMenu == &tmissionModule) {
//			if (strcmp(currentMenu->items[currentIndex], "Resistance") == 0) {
//				valueToAdjust = &resist;
//				strcpy(labelToShow, "Adjust resistance");
//				minVal = 50;
//				maxVal = 200;
//			} else if (strcmp(currentMenu->items[currentIndex], "Reactance") == 0) {
//				valueToAdjust = &react;
//				strcpy(labelToShow, "Adjust reactance");
//				minVal = 0;
//				maxVal = 100;
//			}
//		}
//		else if (currentMenu == &genModule) {
//			if (strcmp(currentMenu->items[currentIndex], "Active") == 0) {
//				valueToAdjust = &active;
//				strcpy(labelToShow, "Adjust active");
//				minVal = 70;
//				maxVal = 200;
//			} else if (strcmp(currentMenu->items[currentIndex], "Reactive") == 0) {
//				valueToAdjust = &reactive;
//				strcpy(labelToShow, "Adjust reactive");
//				minVal = 0;
//				maxVal = 50;
//			}
//		}
//
//		if (valueToAdjust != NULL) {
//			osSemaphoreRelease(AdjustSemHandle); // Start adjustment task
//			printf("Entering adjustment mode for %s\n", labelToShow); // DEBUG PRINT
//		}
//		else {
//		  if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) {
//			handleSelection();
//			osDelay(50);
//			osMutexRelease(displayMutexHandle);
//		  }
//		}
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
		SIM_Transmit(0, clear);

		self.game.time = 0;
		self.game.frequency = 0;
		self.game.mode = SIM_FREQUENCY_GAME;

		SIM_MESSAGE request;
		request.meta.type = SIM_REQUEST;
		request.meta.path = 0;
		request.meta.location = 0;
		for(;;) {
			// clear rx buffer
			ports[0].rx_ctr = 0;
			for (uint8_t msg = 0; msg < 16; msg++) {
				for (uint8_t byte = 0; byte < SIM_MSG_LEN; byte++) {
					ports[0].rx_buf[msg].buffer[byte] = 0;
				}
			}

			// send request
			self.game.time++;
			request.data.request = self.game;
			SIM_Transmit(0, request);
			osDelay(500);

			self.other.master.realgen = 0;
			self.other.master.reactivegen = 0;
			self.other.master.realload = 0;
			self.other.master.reactiveload = 0;

			for (uint8_t loc = 0; loc < 16; loc++) {
				SIM_MESSAGE tile = ports[0].rx_buf[loc];
				switch (tile.meta.origin) {
					case SIM_CCGT: {
						self.other.master.realgen += tile.data.response.bus.real;
						self.other.master.reactivegen += tile.data.response.bus.reactive;
						break;
					}
					case SIM_NUCLEAR: {
						self.other.master.realgen += tile.data.response.bus.real;
						self.other.master.reactivegen += tile.data.response.bus.reactive;
						break;
					}
					case SIM_WIND: {
						self.other.master.realgen += tile.data.response.bus.real;
						self.other.master.reactivegen += tile.data.response.bus.reactive;
						break;
					}
					case SIM_SOLAR: {
						self.other.master.realgen += tile.data.response.bus.real;
						self.other.master.reactivegen += tile.data.response.bus.reactive;
						break;
					}
					case SIM_CITY: {
						self.other.master.realload += tile.data.response.bus.real;
						self.other.master.reactiveload += tile.data.response.bus.reactive;
						break;
					}
					case SIM_FACTORY: {
						self.other.master.realload += tile.data.response.bus.real;
												self.other.master.reactiveload += tile.data.response.bus.reactive;
						break;
					}
					case SIM_CARPARK: {
						self.other.master.realload += tile.data.response.bus.real;
						self.other.master.reactiveload += tile.data.response.bus.reactive;
						break;
					}
					case SIM_PUMPEDHYDRO: {
						if (tile.data.response.bus.real > 0) {
							self.other.master.realgen += tile.data.response.bus.real;
							self.other.master.reactivegen += tile.data.response.bus.reactive;
						} else {
							self.other.master.realload += tile.data.response.bus.real;
							self.other.master.reactiveload += tile.data.response.bus.reactive;
						}
						break;
					}
					case SIM_LITHIUM: {
						if (tile.data.response.bus.real > 0) {
							self.other.master.realgen += tile.data.response.bus.real;
							self.other.master.reactivegen += tile.data.response.bus.reactive;
						} else {
							self.other.master.realload += tile.data.response.bus.real;
							self.other.master.reactiveload += tile.data.response.bus.reactive;
						}
						break;
					}
					case SIM_TRANSMISSION: {
						break;
					}
					case SIM_TRANSFORMER: {
						break;
					}
				}
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
	//AdjustSemHandle
  /* Infinite loop */
  for(;;)
  {
//     // **Wait for a trigger (semaphore)**
//     osSemaphoreAcquire(AdjustSemHandle, osWaitForever);
//
//     while (1) {
//         // Update encoder position
//         encoderPosition = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
//
//         if (encoderPosition > lastEncoderPosition) {
//             *adjustingValue = (*adjustingValue < adjustMax) ? *adjustingValue + 1 : adjustMax;
//         }
//         else if (encoderPosition < lastEncoderPosition) {
//             *adjustingValue = (*adjustingValue > adjustMin) ? *adjustingValue - 1 : adjustMin;
//         }
//         lastEncoderPosition = encoderPosition;
//
//         // Display Adjustment Screen
//         if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) {
//             ssd1306_Fill(Black);
//             ssd1306_SetCursor(10, 10);
//             ssd1306_WriteString(adjustLabel, Font_11x18, White);
//             ssd1306_SetCursor(10, 30);
//
//             char buffer[10];
//             sprintf(buffer, "%d", *adjustingValue);
//             ssd1306_WriteString(buffer, Font_11x18, White);
//
//             ssd1306_SetCursor(10, 60);
//             ssd1306_WriteString("Exit: Press Button", Font_6x8, White);
//
//             ssd1306_UpdateScreen();
//             osMutexRelease(displayMutexHandle);
//         }
//
//         // **Exit if button is pressed**
//         if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14) == GPIO_PIN_RESET) {
//             osDelay(200);  // Debounce delay
//             break;
//         }
//
//         osDelay(50);
//     }
//
//	  // Reset adjustment mode
//	  navigateBack();
      osDelay(1);
  }
  /* USER CODE END ValueTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


// **Trigger Adjustment Mode**
void StartAdjustmentMode(int *value, const char *label, int min, int max) {
    adjustingValue = value;
    adjustLabel = label;
    adjustMin = min;
    adjustMax = max;

    // **Wake up the Adjustment Task**
    osSemaphoreRelease(AdjustSemHandle);
}
/* USER CODE END Application */

