/*
 * sim_game.h
 *
 *  Created on: Mar 27, 2025
 *      Author: vikra
 */

#ifndef INC_SIM_GAME_H_
#define INC_SIM_GAME_H_

#include <stm32h5xx_hal.h>
#include <cmsis_os2.h>

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern osMessageQueueId_t ReceiveQueueHandle;

extern osMessageQueueId_t TransmitQueue1Handle;
extern osMessageQueueId_t TransmitQueue2Handle;
extern osMessageQueueId_t TransmitQueue3Handle;

#define PORT1							&huart1
#define PORT2							&huart2
#define PORT3							&huart3
#define MASTER_PORT				&huart3

#define RX_QUEUE 					&ReceiveQueueHandle

#define TX_QUEUE1 				&TransmitQueue1Handle
#define TX_QUEUE2 				&TransmitQueue2Handle
#define TX_QUEUE3 				&TransmitQueue3Handle

UART_HandleTypeDef *ports[3] =  {PORT1, PORT2, PORT3};
osMessageQueueId_t *queues[3] = {TX_QUEUE1, TX_QUEUE2, TX_QUEUE3};

typedef struct {
	uint8_t pos : 4;
	uint16_t path;
} ADDRESS;

typedef struct {
	uint8_t type;
	uint8_t origin;
	uint8_t last;
	ADDRESS address;
} META_DATA;

typedef struct {
	uint8_t mode;
	uint8_t hour;
	uint8_t minute;
	int8_t frequency;
} UPDATE_DATA;

typedef union {
	struct {
		int16_t real;
		int16_t reactive;
	} bus;

	struct {
		uint8_t length;
		uint8_t quality;
	} line;

	struct {
		int8_t tap;
	} transformer;
} REQUEST_DATA;

typedef union {
	struct {
		int8_t voltage;
		int16_t reactive;
	} bus;

	struct {
		int16_t mva;
	} line;
} RESPONSE_DATA;

typedef struct {
	META_DATA meta;
	union {
		UPDATE_DATA update;
		REQUEST_DATA request;
		RESPONSE_DATA response;
	} data;
} MSG_DATA;

typedef struct {
	uint8_t mode;
	uint8_t hour;
	uint8_t minute;
	float frequency;
} GAME;

#endif /* INC_SIM_GAME_H_ */
