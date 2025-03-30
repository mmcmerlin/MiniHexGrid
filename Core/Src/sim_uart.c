/*
 * sim_uart.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Vikram Barr
 */

#include <string.h>

#include <sim_uart.h>

uint8_t tx_busy[NUM_PORTS] = { 0 };

uint8_t rx_buffer[NUM_PORTS * MAX_MSG_SIZE] = { 0 };
uint8_t tx_buffer[NUM_PORTS * MAX_MSG_SIZE] = { 0 };

uint8_t __UART_FindPort(UART_HandleTypeDef *huart) {
	for (uint8_t port = 0; port < NUM_PORTS; port++) {
		if (ports[port] == huart) return port;
	}
	return 0;
}

void __UART_StartTransmit(MSG_EVENT event) {
	memcpy(&tx_buffer[event.port * MAX_MSG_SIZE], event.msg, event.size);
	HAL_UART_Transmit_DMA(ports[event.port], (uint8_t*) &tx_buffer[event.port * MAX_MSG_SIZE], event.size);
	tx_busy[event.port] = 1;
}

void UART_Init() {
	for (uint8_t port = 0; port < NUM_PORTS; port++) {
		HAL_UARTEx_ReceiveToIdle_DMA(ports[port], (uint8_t*) &rx_buffer[port * MAX_MSG_SIZE], MAX_MSG_SIZE);
	}
}

MSG_EVENT UART_Receive() {
	MSG_EVENT event;
	osMessageQueueGet(*RX_QUEUE, &event, NULL, osWaitForever);
	return event;
}

uint32_t UART_ReceiveQueueCount() {
	return osMessageQueueGetCount(*RX_QUEUE);
}

void UART_Transmit(MSG_EVENT event) {
	if (tx_busy[event.port]) {
		osMessageQueuePut(*queues[event.port], (MSG_EVENT*) &event, 0, 0);
	} else {
		__UART_StartTransmit(event);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	MSG_EVENT event = {	.port=__UART_FindPort(huart), .size=Size	};

	memcpy(&event.msg, huart->pRxBuffPtr, Size);

	osMessageQueuePut(*RX_QUEUE, &event, 0, 0);
	HAL_UARTEx_ReceiveToIdle_DMA(huart, huart->pRxBuffPtr, MAX_MSG_SIZE);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	uint8_t port = __UART_FindPort(huart);
	tx_busy[port] = 0;

	MSG_EVENT event;
	if (osMessageQueueGet(*queues[port], &event, NULL, 0) == osOK) {
		__UART_StartTransmit(event);
	}
}
