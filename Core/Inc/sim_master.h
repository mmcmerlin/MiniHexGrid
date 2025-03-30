/*
 * sim_master.h
 *
 *  Created on: Mar 29, 2025
 *      Author: vikra
 */

#ifndef INC_SIM_MASTER_H_
#define INC_SIM_MASTER_H_

#ifdef MASTER_ACTIONS

typedef struct {

} MASTER_VARS;

void MASTER_Init(void* data) {
	MODULE *module = (MODULE*) data;
	module->base = (MASTER_VARS*) calloc(1, sizeof(MASTER_VARS));
}

void MASTER_Clear(void* data, MSG_EVENT clear) {
	MODULE *module = (MODULE*) data;
	MASTER_VARS* vars = (MASTER_VARS*) module->base;
}

void MASTER_Update(void* data, MSG_EVENT update) {
	MODULE *module = (MODULE*) data;
	MASTER_VARS *vars = (MASTER_VARS*) module->base;
}

void MASTER_Request(void* data, MSG_EVENT request) {
	MODULE *module = (MODULE*) data;
	MASTER_VARS* vars = (MASTER_VARS*) module->base;
}

void MASTER_Response(void* data, MSG_EVENT response) {
	MODULE *module = (MODULE*) data;
	MASTER_VARS* vars = (MASTER_VARS*) module->base;
}

void MASTER_MainLoop(void* data) {
	MODULE *module = (MODULE*) data;

	module->baseInit = &MASTER_Init;
	module->baseHandler[CLEAR] = &MASTER_Clear;
	module->baseHandler[UPDATE] = &MASTER_Update;
	module->baseHandler[REQUEST] = &MASTER_Request;
	module->baseHandler[RESPONSE] = &MASTER_Response;

	for (;;) {
		osDelay(500);

		while (UART_ReceiveQueueCount()) {
			MSG_EVENT event = UART_Receive();

			MSG_DATA *msg = (MSG_DATA*) event.msg;
			module->baseHandler[msg->meta.type](module, event);
		}
	}
}

#endif /* MASTER_ACTIONS */

#endif /* INC_SIM_MASTER_H_ */
