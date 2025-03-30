/*
 * sim_common.h
 *
 *  Created on: Mar 28, 2025
 *      Author: Vikram Barr
 */

#ifndef INC_SIM_BASE_H_
#define INC_SIM_BASE_H_

#include <stdlib.h>

#include <sim.h>
#include <sim_uart.h>
#include <neopixel_driver.h>

#ifdef BASE_ACTIONS

typedef struct {
	uint8_t mapped;
	MODULE_ADDR address;

	uint8_t deactivated;
	uint8_t up, left, right;
} BASE_VARS;

void BASE_Init(void* data) {
	MODULE *module = (MODULE*) data;
	module->base = (BASE_VARS*) calloc(1, sizeof(BASE_VARS));

	module->customInit(data);
}

void BASE_onClear(void* data, MSG_EVENT clear) {
	MODULE *module = (MODULE*) data;
	BASE_VARS *vars = (BASE_VARS*) module->base;

	if (vars->mapped) {
		clear.port = vars->left;
		UART_Transmit(clear);

		clear.port = vars->right;
		UART_Transmit(clear);

		vars->mapped = 0;
	}
}

void BASE_onUpdate(void* data, MSG_EVENT update) {
	MODULE *module = (MODULE*) data;
	BASE_VARS *vars = (BASE_VARS*) module->base;

	uint8_t port = update.port;
	MSG_DATA *msg = (MSG_DATA*) update.msg;

	if (!vars->mapped) {
		vars->address = msg->meta.address;

		vars->up = port;
		vars->left = (port + 1) % 3;
		vars->right = (port + 2) % 3;

		vars->mapped = 1;
	}

	if (module->id.volts == ANY || msg->meta.last.volts == ANY || module->id.volts == msg->meta.last.volts) {
		vars->deactivated = 0;
		NEOPIXEL_SetColour(port + 1, 0, 255, 0);
	} else {
		vars->deactivated = 1;
		NEOPIXEL_SetColour(port + 1, 255, 0, 0);
	}
	NEOPIXEL_Update();


	msg->meta.last = module->id;
	msg->meta.address.pos++;

	update.port = vars->left;
	msg->meta.address.path &= 0x01 << (msg->meta.address.pos - 1);
	UART_Transmit(update);

	update.port = vars->right;
	msg->meta.address.path |= 0x01 << (msg->meta.address.pos - 1);
	UART_Transmit(update);

	if (!vars->deactivated) {

		MSG_EVENT request = {.port=vars->up, .size=sizeof(MSG_DATA)};
		MSG_DATA *msg = (MSG_DATA*) request.msg;
		msg->meta.type = REQUEST;
		msg->meta.address = vars->address;
		msg->meta.origin = module->id;
		msg->meta.last = module->id;

		module->customData[REQUEST](data, &request);

		UART_Transmit(request);
	}
}

void BASE_onRequest(void* data, MSG_EVENT request) {
	MODULE *module = (MODULE*) data;
	BASE_VARS* vars = (BASE_VARS*) module->base;

	uint8_t port = request.port;
	MSG_DATA *msg = (MSG_DATA*) request.msg;

	request.port = vars->up;
	UART_Transmit(request);

	if (module->id.volts == ANY || msg->meta.last.volts == ANY || module->id.volts == msg->meta.last.volts) {
		vars->deactivated = 0;
		NEOPIXEL_SetColour(port + 1, 0, 255, 0);
	} else {
		vars->deactivated = 1;
		NEOPIXEL_SetColour(port + 1, 255, 0, 0);
	}
	NEOPIXEL_Update();
}

void BASE_onResponse(void* data, MSG_EVENT response) {
	MODULE *module = (MODULE*) data;
	BASE_VARS* vars = (BASE_VARS*) module->base;

	MSG_DATA *msg = (MSG_DATA*) response.msg;

	if (msg->meta.address.pos) {
		msg->meta.address.pos--;
		uint8_t dir = msg->meta.address.path & 0x01;
		response.port = (vars->right * dir) + (vars->left * !dir);
		msg->meta.address.path >>= 1;

		UART_Transmit(response);
	} else {
		module->customReceive[RESPONSE](data, $response)
	}
}

void BASE_MainLoop(void* data) {
	MODULE *module = (MODULE*) data;

	module->baseInit(data);

	for (;;) {
		MSG_EVENT event = UART_Receive();

		MSG_DATA *msg = (MSG_DATA*) event.msg;
		module->baseReceive[msg->meta.type](module, event);
	}
}

#endif /* BASE_ACTIONS */

#endif /* INC_SIM_BASE_H_ */
