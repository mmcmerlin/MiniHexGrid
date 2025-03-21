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
#include "cmsis_os2.h"

/*
 * Defines
 */
#define SIM_MSG_LEN					10

#define SIM_MASTER_PORT			2

// Game constants
#define SIM_FREQUENCY				50

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

/*
 * @brief Metadata for all communication between modules.
 */
typedef struct {
	uint8_t type; 					/* Message type: CLEAR, REQUEST, RESPONSE, or UPDATE. */
	uint8_t location;				/* Distance between master and module.								*/
	uint16_t path;					/* Path to the module, 0 -> Right, 1 -> Left. 				*/
	uint8_t origin;					/* Type of module that generated this message. 				*/
	uint8_t last;						/* Last module to send this message.									*/
} SIM_METADATA;

/*
 * @brief Data for periodic REQUEST messages sent from the Master.
 */
typedef struct {
	uint8_t mode;						/* Gamemode (Not implemented) 												*/
	uint8_t time;						/* In-game time | 0-240, time / 10 = hour 						*/
	int8_t frequency;				/* Frequency | (frequency / 255) + 1 = p.u. frequency */
} SIM_REQUEST_DATA;

/*
 * @brief Data for a module's response to the Master.
 */
typedef union {
	struct {
		int16_t real;					// real / 10 = MW real power
		int16_t reactive;			// reactive / 10 = MVAr reactive power
	} bus;

	struct {
		uint8_t type;					// 0 - 50km low quality, 1 - 10km high quality
	} line;

	struct {
		int8_t tap;						// (tap / 255) + 1 = tap ratio
	} transformer;
} SIM_RESPONSE_DATA;

/*
 * @brief Data for an information update coming from the Master.
 */
typedef union {
	struct {
		int8_t voltage;				// (voltage / 255) + 1 = pu voltage
		int16_t reactive;			// reactive / 10 = MVAr reactive power
	} bus;

	struct {
		int16_t mva;					// mva / 10 = MVA line flow
	} line;

	struct {
		int16_t mva;					// mva / 10 = MVA transformer flow
	} transformer;
} SIM_UPDATE_DATA;

/*
 * @brief General message structure that can be accessed differently depending on need.
 */
typedef union {
	struct {
		SIM_METADATA meta;
		union {
			SIM_REQUEST_DATA request;
			SIM_RESPONSE_DATA response;
			SIM_UPDATE_DATA update;
		} data;
	};
	uint8_t buffer[SIM_MSG_LEN];
} SIM_MESSAGE;

/*
 * @brief Struct to be stored in queues for messages.
 */
typedef struct {
	UART_HandleTypeDef *huart;
	SIM_MESSAGE *message;
} SIM_EVENT;

/*
 * @brief Extra module-dependent data to be stored locally and not transmitted.
 */
typedef union {
	struct {
		int16_t setpoint;			// setpoint / 10 = MW real power
		uint8_t delta;
	} ccgt;

	struct {
		int16_t setpoint;			// setpoint / 10 = MW real power
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
		int16_t setpoint;			// setpoint / 10 = MW real power
		uint8_t delta;
	} factory;

	struct {
		int16_t volume;				// volume / 10 = max MWh stored
		int16_t stored;				// stored / 10 = MWh stored
		int16_t setpoint;			// setpoint / 10 = MW real power
		uint8_t delta;
	} phydro;

	struct {
		int16_t capacity;			// capacity / 10 = MWh per cell
		int16_t stored;				// stored / 10 = MWh stored
		uint8_t cells;				// number of cells
	} lithium;

	struct {
		float realgen;
		float reactivegen;
		float realload;
		float reactiveload;
	} master;
} SIM_LOCAL_DATA;

typedef struct {
	uint8_t module;
	uint8_t location;
	uint16_t path;

	uint8_t mapped;
	uint8_t upstream;
	uint8_t left;
	uint8_t right;

	uint8_t disabled;

	SIM_REQUEST_DATA game;
	SIM_RESPONSE_DATA local;
	SIM_UPDATE_DATA received;
	SIM_LOCAL_DATA other;
} SIM_DATA;

typedef struct {
	UART_HandleTypeDef *huart;
	osMessageQueueId_t *tx_queue;

	SIM_MESSAGE rx_buf[16];
	uint8_t rx_ctr : 4;

	SIM_MESSAGE tx_buf[16];
	uint8_t tx_ctr : 4;
} SIM_PORT;

void SIM_Init();

void SIM_Transmit(uint8_t port, SIM_MESSAGE message);

uint8_t SIM_FindPort(UART_HandleTypeDef *huart);

uint8_t SIM_Compatible(uint8_t type1, uint8_t type2);

int16_t SIM_RandInt(int16_t lower, int16_t upper);

float SIM_GetFrequency();

void SIM_SetFrequency(float frequency);


#endif /* INC_SIM_DRIVER_H_ */
