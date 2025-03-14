/*
 * sim_driver.h
 *
 *  Created on: Mar 3, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#ifndef INC_SIM_DRIVER_H_
#define INC_SIM_DRIVER_H_

/*
 * Includes
 */
#include "stm32h5xx_hal.h"

/*
 * Defines
 */
#define SIM_MSG_LEN					10

// Message Types
#define SIM_CLEAR						0
#define SIM_REQUEST					1
#define SIM_RESPONSE				2
#define	SIM_UPDATE					3

// Module Types
#define SIM_MASTER					0

#define SIM_TRANSMISSION		1
#define SIM_TRANSFORMER			2

#define SIM_CCGT						4
#define SIM_NUCLEAR					5
#define SIM_WIND						6
#define SIM_SOLAR						7

#define SIM_CITY						8
#define SIM_FACTORY					9
#define SIM_DATACENTER			10

#define SIM_PUMPEDHYDRO			12
#define SIM_LITHIUM					13

// Game types
#define SIM_FREQUENCY_GAME	0

/*
 * Message Protocol
 */
typedef struct {
	uint8_t type;
	uint8_t location;
	uint16_t path;
	uint8_t origin;
	uint8_t last;
} SIM_METADATA;

typedef struct {
	uint8_t mode;
	int8_t time;
	int8_t frequency;
} SIM_REQUEST_DATA;

typedef union {
	struct {
		int16_t real;
		int16_t reactive;
	} bus;

	struct {
		uint16_t length;
	} line;

	struct {
		int16_t tap;
	} transformer;

	struct {
		int16_t capacity;
	} storage;
} SIM_RESPONSE_DATA;

typedef union {
	struct {
		int8_t voltage;
		int16_t reactive;
	} bus;

	struct {
		int16_t mva;
	} line;

	struct {
		int16_t mva;
	} transformer;

	struct {
		int16_t real;
		int16_t reactive;
	} storage;
} SIM_UPDATE_DATA;

typedef union {
	SIM_REQUEST_DATA request;
	SIM_RESPONSE_DATA response;
	SIM_UPDATE_DATA update;
} SIM_MESSAGE_DATA;

typedef struct {
	SIM_METADATA meta;
	SIM_MESSAGE_DATA data;
} SIM_MESSAGE;

typedef struct {
	UART_HandleTypeDef *huart;
	SIM_MESSAGE *message;
} SIM_EVENT;

typedef struct {
	uint8_t module;
	uint8_t location;
	uint16_t path;
	uint8_t mapped;
	uint8_t upstream;
	uint8_t left;
	uint8_t right;
	SIM_REQUEST_DATA game;
	SIM_RESPONSE_DATA local;
	SIM_UPDATE_DATA received;
} SIM_LOCAL_DATA;

typedef struct {
	UART_HandleTypeDef *huart;
	osMessageQueueId_t *tx_queue;
	SIM_MESSAGE rx_buf[16];
	SIM_MESSAGE tx_buf[16];
	uint8_t rx_ctr : 4;
	uint8_t tx_ctr : 4;
} SIM_PORT;

#endif /* INC_SIM_DRIVER_H_ */
