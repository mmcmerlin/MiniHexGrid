/*
 * sim_uart.h
 *
 *  Created on: Mar 3, 2025
 *      Author: Vikram Barr
 */

#ifndef INC_SIM_UART_H_
#define INC_SIM_UART_H_

#include <stm32h5xx_hal.h>
#include <cmsis_os2.h>

#define NUM_PORTS					3
#define MAX_MSG_SIZE 			64

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
	uint8_t port;
	uint16_t size;
	uint8_t msg[MAX_MSG_SIZE];
} MSG_EVENT;

void UART_Init();

void UART_Transmit(MSG_EVENT event);

MSG_EVENT UART_Receive();

uint32_t UART_ReceiveQueueCount();

#endif /* INC_SIM_UART_H_ */
