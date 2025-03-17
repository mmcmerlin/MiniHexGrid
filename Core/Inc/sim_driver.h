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
#define SIM_CARPARK					10

#define SIM_PUMPEDHYDRO			12
#define SIM_LITHIUM					13

// Game types
#define SIM_FREQUENCY_GAME	0

/*
 * Message Protocol
 */
typedef struct {
	uint8_t type; 					// message type
	uint8_t location;				// part of the address, the number of modules away from the master
	uint16_t path;					// part of the address, series of 0's (right) and 1's (left)
	uint8_t origin;					// type of module that originated the message
	uint8_t last;						// type of last module to send this message (a neighbor)
} SIM_METADATA;

typedef struct {
	uint8_t mode;						// game mode
	int8_t time;					// game time | negative: night, positive: day
	int8_t frequency;				// (frequency / 255) + 1 = current frequency / rated frequency
} SIM_REQUEST_DATA;

typedef union {
	struct {
		int16_t real;					// real / 100 = MW real power
		int16_t reactive;			// reactive / 100 = MVAr reactive power
	} bus;

	struct {
		uint8_t type;					// 0 - 10km high quality, 1 - 50km low quality
	} line;

	struct {
		int8_t tap;						// (tap / 255) + 1 = tap ratio
	} transformer;
} SIM_RESPONSE_DATA;

typedef union {
	struct {
		int8_t voltage;				// (voltage / 255) + 1 = pu voltage
		int16_t reactive;			// reactive / 100 = MVAr reactive power
	} bus;

	struct {
		int16_t mva;					// mva / 100 = MVA line flow
	} line;

	struct {
		int16_t mva;					// mva / 100 = MVA transformer flow
	} transformer;
} SIM_UPDATE_DATA;

typedef union {
	struct {
		int16_t setpoint;			// setpoint / 100 = MW real power
		uint8_t delta;
	} ccgt;

	struct {
		int16_t setpoint;			// setpoint / 100 = MW real power
		uint8_t delta;
	} nuclear;

	struct {
		uint8_t	rating;				// rating / 10 = maximum turbine MW
		uint8_t turbines;			// turbines * rating = maximum farm MW
		int16_t speed;				// speed / 10 = ms^-1
		uint8_t rated;				// rated / 10 = speed at which rating MW is produced
		uint8_t cutout;				// cutout / 10 = cut out speed
		uint8_t cutin;				// cutin / 10 = cut in speed
	} wind;

	struct {
		uint8_t intensity;		// watts per m^2
		uint16_t area;				// area in m^2
	} solar;

	struct {
		uint16_t population;	// population * 1000 = actual population | actual population / 1000 = MW consumption
	} city;

	struct {
		uint8_t spaces;				// number of charging spaces | spaces * 2 = MW consumption
	} carpark;

	struct {
		int16_t setpoint;			// setpoint / 100 = MW real power
		uint8_t delta;
	} factory;

	struct {
		int16_t volume;				// volume / 10 = max MWh stored
		int16_t stored;				// stored / 10 = MWh stored
		int16_t setpoint;			// setpoint / 100 = MW real power
		uint8_t delta;
	} phydro;

	struct {
		int16_t capacity;			// capacity / 10 = MWh per cell
		int16_t stored;				// stored / 10 = MWh stored
		uint8_t cells;				// number of cells
	} lithium;

	struct {

	} transmission;

	struct {

	} transformer;
} SIM_LOCAL_DATA;

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
	SIM_LOCAL_DATA other;
} SIM_DATA;

typedef struct {
	UART_HandleTypeDef *huart;
	osMessageQueueId_t *tx_queue;
	SIM_MESSAGE rx_buf[16];
	SIM_MESSAGE tx_buf[16];
	uint8_t rx_ctr : 4;
	uint8_t tx_ctr : 4;
} SIM_PORT;

#endif /* INC_SIM_DRIVER_H_ */
