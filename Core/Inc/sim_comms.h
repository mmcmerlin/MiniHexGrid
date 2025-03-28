/*
 * sim_driver.h
 *
 *  Created on: Mar 3, 2025
 *      Author: Vikram Barr
 */

#ifndef INC_SIM_COMMS_H_
#define INC_SIM_COMMS_H_

/*
 * Includes
 */
#include <sim_game.h>

#include <stdlib.h>

#include <stm32h5xx_hal.h>
#include <cmsis_os2.h>

typedef struct {
	uint8_t port;
	MSG_DATA msg;
} RX_EVENT;

typedef struct {
	MSG_DATA msg;
} TX_EVENT;

void COMMS_Init();

void COMMS_Transmit(uint8_t port, MSG_DATA message);

#endif /* INC_SIM_COMMS_H_ */
