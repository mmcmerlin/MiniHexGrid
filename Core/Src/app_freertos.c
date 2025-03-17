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
SIM_PORT SIM_PORTS[3] = {
		{.huart = &huart1, .tx_queue = &UARTTx1QueueHandle, .rx_ctr = 0, .tx_ctr = 0},
		{.huart = &huart2, .tx_queue = &UARTTx2QueueHandle, .rx_ctr = 0, .tx_ctr = 0},
		{.huart = &huart3, .tx_queue = &UARTTx3QueueHandle, .rx_ctr = 0, .tx_ctr = 0},
};

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
  EncoderTaskHandle = osThreadNew(StartEncoderTask, NULL, &EncoderTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

  /* creation of ServoTask */
  ServoTaskHandle = osThreadNew(StartServoTask, NULL, &ServoTask_attributes);

  /* creation of UARTTask */
  UARTTaskHandle = osThreadNew(StartUARTTask, NULL, &UARTTask_attributes);

  /* creation of ButtonTask */
  ButtonTaskHandle = osThreadNew(StartButtonTask, NULL, &ButtonTask_attributes);

  /* creation of GameTask */
  GameTaskHandle = osThreadNew(StartGameTask, NULL, &GameTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartNeoPixelTask */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	NEOPIXEL_Callback();
}

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
	NEOPIXEL_SetColour(1, 255, 0, 0);
	NEOPIXEL_SetColour(2, 0, 255, 0);
	NEOPIXEL_SetColour(3, 0, 0, 255);
	NEOPIXEL_Update();
	osDelay(500);
	NEOPIXEL_SetColour(1, 0, 255, 0);
	NEOPIXEL_SetColour(2, 0, 0, 255);
	NEOPIXEL_SetColour(3, 255, 0, 0);
	NEOPIXEL_Update();
	osDelay(500);
	NEOPIXEL_SetColour(1, 0, 0, 255);
	NEOPIXEL_SetColour(2, 255, 0, 0);
	NEOPIXEL_SetColour(3, 0, 255, 0);
	NEOPIXEL_Update();
	osDelay(500);
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
  /* Infinite loop */
  for(;;)
  {
	if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) {
		updateMenuDisplay();
		osMutexRelease(displayMutexHandle);
	}
	osDelay(40);
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
	 /*Create routines*/
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 500);   // Move to -90° ~1ms pulse
	osDelay(1000);
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4, 2500); // Move to 90° ~2ms pulse
	osDelay(1000);
  }
  /* USER CODE END ServoTask */
}

/* USER CODE BEGIN Header_StartUARTTask */
void SIM_Init(SIM_DATA obj) {
	// TODO read from dip switches to find module type
	obj.module = SIM_MASTER;
	obj.mapped = 0;

	switch (obj.module) {
	case SIM_CCGT:
		obj.other.ccgt.setpoint = 1500; 			// 150 MW
		obj.other.ccgt.delta = 200; 					// 200 / 255
		break;
	case SIM_NUCLEAR:
		obj.other.nuclear.setpoint = 6000; 		// 600 MW
		obj.other.nuclear.delta = 10; 				// 10 / 255
		break;
	case SIM_WIND:
		obj.other.wind.speed = 50; 						// 5 ms^-1
		obj.other.wind.turbines = 100;
		obj.other.wind.rating = 50; 					// 5 MW
		obj.other.wind.rated = 130; 					// 13 ms^-1
		obj.other.wind.cutin = 20; 						// 2 ms^-1
		obj.other.wind.cutout = 220; 					// 22 ms^-1
		break;
	case SIM_SOLAR:
		obj.other.solar.area = 5000; 					// 5000 m^2
		break;
	case SIM_CITY:
		obj.other.city.population = 1000; 		// 1 million people = 1000 MW
		break;
	case SIM_FACTORY:
		obj.other.factory.setpoint = -1000; 	// -100 MW
		obj.other.factory.delta = 150; 				// 150 / 255
		break;
	case SIM_CARPARK:
		obj.other.carpark.spaces = 200;
		break;
	case SIM_PUMPEDHYDRO:
		obj.other.phydro.setpoint = 0;				// 0 MW
		obj.other.phydro.delta = 150;					// 150 / 255
		obj.other.phydro.stored = 0;
		break;
	case SIM_LITHIUM:
		obj.other.lithium.capacity = 1;				// 100kWh / cell
		obj.other.lithium.cells = 20;
		obj.other.lithium.stored = 0;
		break;
	}

	// begin recursive DMA receives for every port
	for (uint8_t port = 0; port < 3; port++) {
		HAL_UARTEx_ReceiveToIdle_DMA(SIM_PORTS[port].huart, (uint8_t*) SIM_PORTS[port].rx_buf, SIM_MSG_LEN);
	}


}

uint8_t SIM_UART_FindPort(UART_HandleTypeDef *huart) {
	for (uint8_t port = 0; port < 3; port++) {
		if (SIM_PORTS[port].huart == huart) return port;
	}
	return 0;
}

void SIM_UART_Transmit(uint8_t port, SIM_MESSAGE message) {
	// save the message to the port tx buffer
	uint8_t buf_ctr = SIM_PORTS[port].tx_ctr++;
	SIM_PORTS[port].tx_buf[buf_ctr] = message;

	// attempt to transmit message, and if it fails, add it to the queue
	if (HAL_UART_Transmit_DMA(SIM_PORTS[port].huart, (uint8_t *) &SIM_PORTS[port].tx_buf[buf_ctr], SIM_MSG_LEN) == HAL_BUSY) {
		SIM_EVENT event = {.huart = SIM_PORTS[port].huart, .message = &SIM_PORTS[port].tx_buf[buf_ctr]};
		osMessageQueuePut(*SIM_PORTS[port].tx_queue, &event, 0, 0);
	}
}

int16_t SIM_RandInt(int16_t lower, int16_t upper) {
	uint32_t random;
	HAL_RNG_GenerateRandomNumber(&hrng, (uint32_t *) &random);

	return (random % (upper - lower)) + lower;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (Size == SIM_MSG_LEN) {
  	// add the message to the mail queue
		SIM_EVENT event = { .huart = huart, .message = (SIM_MESSAGE *) huart->pRxBuffPtr };
		osMessageQueuePut(UARTMailQueueHandle, &event, 0, 0);

		// start listening again on an incremented location in the port's rx buffer
		uint8_t port = SIM_UART_FindPort(huart);
		HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *) &SIM_PORTS[port].rx_buf[SIM_PORTS[port].rx_ctr++], SIM_MSG_LEN);
  } else {
  	// if the message is incomplete, it can be overwritten in the port's rx buffer
  	HAL_UARTEx_ReceiveToIdle_DMA(huart, huart->pRxBuffPtr, SIM_MSG_LEN);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	SIM_EVENT next;
	uint8_t prio;
	uint8_t port = SIM_UART_FindPort(huart);

	// check if there are messages in the queue and start transmission on the next if so
	if (osMessageQueueGet(*SIM_PORTS[port].tx_queue, &next, &prio, 0) == osOK) {
		HAL_UART_Transmit_DMA(huart, (uint8_t *) next.message, SIM_MSG_LEN);
	}
}

/**
* @brief Function implementing the UARTTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUARTTask */
void StartUARTTask(void *argument)
{
  /* USER CODE BEGIN UARTTask */
	SIM_Init(self);

	/* Infinite loop */
	for (;;) {
		SIM_EVENT event;
		uint8_t priority;
		// wait until a message can be retrieved from the queue
		osMessageQueueGet(UARTMailQueueHandle, &event, &priority, osWaitForever);

		// process the message
		SIM_MESSAGE copy = *event.message;

		if (self.module == SIM_MASTER) {
			if (copy.meta.type == SIM_RESPONSE) {

			}
		} else {
			switch (copy.meta.type) {
			case SIM_CLEAR:
				if (self.mapped) {
					SIM_UART_Transmit(self.left, copy);
					SIM_UART_Transmit(self.right, copy);
					self.mapped = 0;
				}
				break;
			case SIM_REQUEST:
				// assign our ports if we haven't already
				if (!self.mapped) {
					self.path = copy.meta.path;
					self.location = copy.meta.location;

					int8_t port = SIM_UART_FindPort(event.huart);
					self.upstream = port;
					self.left = (port + 1) % 3;
					self.right = (port + 2) % 3;
					self.mapped = 1;
				}

				self.game = copy.data.request;

				// TODO check compatibility with last module
				// relay the message on both ports after editing the address and sender
				copy.meta.last = self.module;
				copy.meta.location++;
				SIM_UART_Transmit(self.left, copy);
				copy.meta.path |= 0x01 << (copy.meta.location - 1);
				SIM_UART_Transmit(self.right, copy);

				// update game state
				switch (self.module) {
					case SIM_CCGT: {
						// generation approaches setpoint
						float real = (float) self.local.bus.real / 100;
						float setpoint = (float) self.other.ccgt.setpoint / 100;
						float ratio = (float) self.other.ccgt.delta / 255;
						self.local.bus.real = (int16_t) (100 * (real + (ratio * (setpoint - real / setpoint))));
						break;
					}
					case SIM_NUCLEAR: {
						// generation approaches setpoint
						float real = (float) self.local.bus.real / 100;
						float setpoint = (float) self.other.nuclear.setpoint / 100;
						float ratio = (float) self.other.nuclear.delta / 255;
						self.local.bus.real = (int16_t) (100 * (real + (ratio * (setpoint - real / setpoint))));
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
							self.local.bus.real = self.other.wind.rating;
						}
						break;
					}
					case SIM_SOLAR: {
						// set new intensity
						if (self.game.time == 0) self.other.solar.intensity = SIM_RandInt(10, 255);
						else if (self.game.time == 127) self.other.solar.intensity = 0;

						// update generation

						break;
					}
					case SIM_CITY: {
						break;
					}
					case SIM_FACTORY: {
						if (self.game.time == 0) self.other.solar.intensity = SIM_RandInt(10, 255);
						if (self.game.time == 127) self.other.solar.intensity = 0;
						break;
					}
					case SIM_CARPARK: {
						// slight random variation
						self.local.bus.real = self.other.carpark.spaces;
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

				SIM_UART_Transmit(self.upstream, response);
				break;
			case SIM_RESPONSE:
				// immediately relay the message to the master
				SIM_UART_Transmit(self.upstream, copy);
				break;
			case SIM_UPDATE:
				// check if we are the recipient
				if (copy.meta.location) {
					copy.meta.location--;
					uint8_t direction = copy.meta.path & 0x01;
					copy.meta.path >>= 1;

					// relay the copy on the correct port
					SIM_UART_Transmit((self.right * direction) + (self.left * !direction), copy);
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
  /* Infinite loop */
  for(;;)
  {
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET && !buttonPressed)
	{
	  buttonPressed = 1;
	  if (osMutexAcquire(displayMutexHandle, osWaitForever) == osOK) {
		handleSelection();
		osMutexRelease(displayMutexHandle);
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
		SIM_UART_Transmit(0, clear);

		self.game.time = 0;
		self.game.frequency = 0;
		self.game.mode = SIM_FREQUENCY_GAME;
		SIM_MESSAGE request;
		request.meta.type = SIM_REQUEST;
		request.meta.path = 0;
		request.meta.location = 0;
		for(;;) {
			self.game.time++;
			request.data.request = self.game;
			SIM_UART_Transmit(0, request);
			osDelay(500);
		}
	} else {
		for(;;) {
			osDelay(1);
		}
	}
  /* USER CODE END GameTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

