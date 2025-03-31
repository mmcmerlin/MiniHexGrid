/*
 * sim_master.c
 *
 *  Created on: Mar 30, 2025
 *      Author: vikra
 */

#include <sim_master.h>

void MASTER_onInit(MODULE *module) {

}

void MASTER_onRequest(MODULE *module, MSG_EVENT request) {

}

void MASTER_Main(MODULE *module) {
	MASTER_Init(data);

	void (*masterReceive[NUM_TYPES])(MODULE*, MSG_EVENT) = {
				[REQUEST] = &MASTER_onRequest,
		};

	for (;;) {
		osDelay(500);

		while (UART_ReceiveQueueCount()) {
			MSG_EVENT event = UART_Receive();

			MSG_DATA *msg = (MSG_DATA*) event.msg;
			masterReceive[msg->meta.type](data, event);
		}
	}
}
