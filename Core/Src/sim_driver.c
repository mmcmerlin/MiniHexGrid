/*
 * sim_driver.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#include "sim_driver.h"

extern SIM_DATA self;
extern SIM_PORT ports[3];

extern RNG_HandleTypeDef hrng;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern osMessageQueueId_t UARTMailQueueHandle;
extern osMessageQueueId_t UARTTx1QueueHandle;
extern osMessageQueueId_t UARTTx2QueueHandle;
extern osMessageQueueId_t UARTTx3QueueHandle;

void SIM_Init() {
	// TODO read from dip switches to find module type
	self.module = SIM_CITY;
	self.mapped = 0;

	switch (self.module) {
	case SIM_MASTER:
		break;
	case SIM_CCGT:
		self.other.ccgt.setpoint = 1500; 				// 150 MW
		self.other.ccgt.delta = 200; 						// 200 / 255

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_NUCLEAR:
		self.other.nuclear.setpoint = 6000; 		// 600 MW
		self.other.nuclear.delta = 50; 					// 10 / 255

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_WIND:
		self.other.wind.speed = 50; 						// 5 ms^-1
		self.other.wind.turbines = 100;
		self.other.wind.rating = 50; 						// 5 MW
		self.other.wind.rated = 130; 						// 13 ms^-1
		self.other.wind.cutin = 20; 						// 2 ms^-1
		self.other.wind.cutout = 220; 					// 22 ms^-1

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_SOLAR:
		self.other.solar.area = 5000; 					// 5000 m^2

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_CITY:
		self.other.city.population = 600; 			// 1 million people = 1000 MW

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_FACTORY:
		self.other.factory.setpoint = -1000; 		// -100 MW
		self.other.factory.delta = 150; 				// 150 / 255

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_CARPARK:
		self.other.carpark.spaces = 200;

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_PUMPEDHYDRO:
		self.other.phydro.setpoint = 0;					// 0 MW
		self.other.phydro.delta = 150;					// 150 / 255
		self.other.phydro.stored = 0;

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_LITHIUM:
		self.other.lithium.capacity = 1;				// 100kWh / cell
		self.other.lithium.cells = 20;
		self.other.lithium.stored = 0;

		self.local.bus.real = 0;
		self.local.bus.reactive = 0;
		break;
	case SIM_TRANSMISSION:
		self.local.line.type = 1;
		break;
	case SIM_TRANSFORMER:
		self.local.transformer.tap = 0;
		break;
	}

	// Begin recursive DMA receives on every port.
	for (uint8_t port = 0; port < 3; port++) {
		HAL_UARTEx_ReceiveToIdle_DMA(ports[port].huart, (uint8_t*) ports[port].rx_buf, SIM_MSG_LEN);
	}
}

uint8_t SIM_FindPort(UART_HandleTypeDef *huart) {
	for (uint8_t port = 0; port < 3; port++) {
		if (ports[port].huart == huart) return port;
	}
	return 0;
}

void SIM_Transmit(uint8_t port, SIM_MESSAGE message) {
	// Save the message to the port tx buffer.
	uint8_t buf_ctr = ports[port].tx_ctr++;
	ports[port].tx_buf[buf_ctr] = message;

	// Attempt to transmit the message, if it fails, add it to the queue.

	if (HAL_UART_Transmit_DMA(ports[port].huart, (uint8_t *) &ports[port].tx_buf[buf_ctr], SIM_MSG_LEN) == HAL_BUSY) {
		SIM_EVENT event = {.huart = ports[port].huart, .message = &ports[port].tx_buf[buf_ctr]};
		osMessageQueuePut(*ports[port].tx_queue, &event, 0, 0);
	}
}

int16_t SIM_RandInt(int16_t lower, int16_t upper) {
	uint32_t random;
	HAL_RNG_GenerateRandomNumber(&hrng, (uint32_t *) &random);

	return (random % (upper - lower)) + lower;
}

const uint8_t SIM_VOLTAGES[16] = {
	4,					// SIM_MASTER						0
	3,					// SIM_TRANSMISSION			1
	4,					// SIM_TRANSFORMER			2
	0,					//
	2,					// SIM_CCGT							4
	2,					// SIM_NUCLEAR					5
	2,					// SIM_WIND							6
	2,					// SIM_SOLAR						7
	1,					// SIM_CITY							8
	1,					// SIM_FACTORY					9
	1,					// SIM_CARPARK					10
	0,					//
	1,					// SIM_PUMPEDHYDRO			12
	1,					// SIM_LITHIUM					13
};

uint8_t SIM_Compatible(uint8_t type1, uint8_t type2) {
	if (SIM_VOLTAGES[type1] == 4 || SIM_VOLTAGES[type2] == 4) return 1;
	if (SIM_VOLTAGES[type1] == SIM_VOLTAGES[type2]) return 1;
	return 0;
}

float SIM_GetFrequency() {
	return (((float) self.game.frequency / 255) + 1) * SIM_FREQUENCY;
}

void SIM_SetFrequency(float frequency) {
	float conversion = (((frequency / SIM_FREQUENCY) - 1) * 255);
	if (conversion > 127) self.game.frequency = 127;
	else if (conversion < -128) self.game.frequency = -128;
	else self.game.frequency = (int8_t) conversion;
	return;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (Size == SIM_MSG_LEN) {
  	// add the message to the mail queue
		SIM_EVENT event = { .huart = huart, .message = (SIM_MESSAGE *) huart->pRxBuffPtr };
		osMessageQueuePut(UARTMailQueueHandle, &event, 0, 0);

		// start listening again on an incremented location in the port's rx buffer
		uint8_t port = SIM_FindPort(huart);
		HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t *) &ports[port].rx_buf[ports[port].rx_ctr++], SIM_MSG_LEN);
  } else {
  	// if the message is incomplete, it can be overwritten in the port's rx buffer
  	HAL_UARTEx_ReceiveToIdle_DMA(huart, huart->pRxBuffPtr, SIM_MSG_LEN);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	SIM_EVENT next;
	uint8_t prio;
	uint8_t port = SIM_FindPort(huart);

	// check if there are messages in the queue and start transmission on the next if so
	if (osMessageQueueGet(*ports[port].tx_queue, &next, &prio, 0) == osOK) {
		HAL_UART_Transmit_DMA(huart, (uint8_t *) next.message, SIM_MSG_LEN);
	}
}
