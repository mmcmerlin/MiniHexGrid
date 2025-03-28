/*
 * sim_comms.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Vikram Barr
 */

#include <sim_comms.h>

MSG_DATA 	rx_buffer[3] = 	{ 0 };
MSG_DATA 	tx_buffer[3] = 	{ 0 };
uint8_t 	tx_busy[3] = 		{ 0 };

uint8_t COMMS_FindPort(UART_HandleTypeDef *huart) {
	for (uint8_t port = 0; port < 3; port++) {
		if (ports[port] == huart)
			return port;
	}
	return 0;
}

void COMMS_Init() {
	for (uint8_t port = 0; port < 3; port++) {
		HAL_UARTEx_ReceiveToIdle_DMA(ports[port], (uint8_t*) &rx_buffer[port], sizeof(MSG_DATA));
	}
}

void COMMS_Transmit(uint8_t port, MSG_DATA message) {
	if (tx_busy[port]) {
		osMessageQueuePut(*queues[port], (TX_EVENT*) &message, 0, 0);
	} else {
		tx_buffer[port] = message;
		HAL_UART_Transmit_DMA(ports[port], (uint8_t*) &tx_buffer[port], sizeof(MSG_DATA));
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if (Size == sizeof(MSG_DATA)) {
		RX_EVENT event = {
			.port = COMMS_FindPort(huart),
			.msg = *(MSG_DATA*) huart->pRxBuffPtr,
		};
		osMessageQueuePut(ReceiveQueueHandle, &event, 0, 0);
	}
	HAL_UARTEx_ReceiveToIdle_DMA(huart, huart->pRxBuffPtr, sizeof(MSG_DATA));
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	uint8_t port = COMMS_FindPort(huart);
	tx_busy[port] = 0;

	TX_EVENT event;
	if (osMessageQueueGet(*queues[port], &event, NULL, 0) == osOK) {
		tx_buffer[port] = event.msg;
		HAL_UART_Transmit_DMA(huart, (uint8_t*) &tx_buffer[port], sizeof(MSG_DATA));
		tx_busy[port] = 1;
	}
}
