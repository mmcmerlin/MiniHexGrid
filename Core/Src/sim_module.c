/*
 * sim_module.c
 *
 *  Created on: Mar 30, 2025
 *      Author: vikra
 */

#include <sim_module.h>

uint8_t mapped;
MODULE_ADDR address;

uint8_t deactivated;
uint8_t up, left, right;

typedef struct {

} GENERAL_VARS;

void MODULE_Init(MODULE *module) {
	MODULE *module = (MODULE*) data;
	module->common = (GENERAL_VARS*) calloc(1, sizeof(GENERAL_VARS));

	module->customInit(module);
}

void MODULE_onClear(MODULE *module, MSG_EVENT clear) {
	if (mapped) {
		clear.port = left;
		UART_Transmit(clear);

		clear.port = right;
		UART_Transmit(clear);

		mapped = 0;
	}
}

void MODULE_onUpdate(MODULE *module, MSG_EVENT update) {
	uint8_t port = update.port;
	MSG_DATA *msg = (MSG_DATA*) update.msg;

	if (!mapped) {
		address = msg->meta.address;

		up = port;
		left = (port + 1) % 3;
		right = (port + 2) % 3;

		mapped = 1;
	}

	if (module->id.volts == ANY || msg->meta.last.volts == ANY || module->id.volts == msg->meta.last.volts) {
		deactivated = 0;
		NEOPIXEL_SetColour(port + 1, 0, 255, 0);
	} else {
		deactivated = 1;
		NEOPIXEL_SetColour(port + 1, 255, 0, 0);
	}
	NEOPIXEL_Update();

	msg->meta.last = module->id;
	msg->meta.address.pos++;

	update.port = left;
	msg->meta.address.path &= 0x01 << (msg->meta.address.pos - 1);
	UART_Transmit(update);

	update.port = right;
	msg->meta.address.path |= 0x01 << (msg->meta.address.pos - 1);
	UART_Transmit(update);

	module->customReceive[REQUEST](module, update);

	if (!deactivated) {

		MSG_EVENT request = {.port=up, .size=sizeof(MSG_DATA)};
		MSG_DATA *msg = (MSG_DATA*) request.msg;
		msg->meta.type = REQUEST;
		msg->meta.address = address;
		msg->meta.origin = module->id;
		msg->meta.last = module->id;

		module->customData[REQUEST](module, &request);

		UART_Transmit(request);
	}
}

void MODULE_onRequest(MODULE *module, MSG_EVENT request) {
	uint8_t port = request.port;
	MSG_DATA *msg = (MSG_DATA*) request.msg;

	request.port = up;
	UART_Transmit(request);

	if (module->id.volts == ANY || msg->meta.last.volts == ANY || module->id.volts == msg->meta.last.volts) {
		deactivated = 0;
		NEOPIXEL_SetColour(port + 1, 0, 255, 0);
	} else {
		deactivated = 1;
		NEOPIXEL_SetColour(port + 1, 255, 0, 0);
	}
	NEOPIXEL_Update();
}

void MODULE_onResponse(MODULE *module, MSG_EVENT response) {
	MSG_DATA *msg = (MSG_DATA*) response.msg;

	if (msg->meta.address.pos) {
		msg->meta.address.pos--;
		uint8_t dir = msg->meta.address.path & 0x01;
		response.port = (right * dir) + (left * !dir);
		msg->meta.address.path >>= 1;

		UART_Transmit(response);
	} else {
		module->customReceive[RESPONSE](module, response);
	}
}

void MODULE_Main(MODULE *module) {
	MODULE_Init(module);

	void (*moduleReceive[NUM_TYPES])(MODULE*, MSG_EVENT) = {
			[CLEAR] = &MODULE_onClear,
			[UPDATE] = &MODULE_onUpdate,
			[REQUEST] = &MODULE_onRequest,
			[RESPONSE] = &MODULE_onResponse,
	};

	for (;;) {
		MSG_EVENT event = UART_Receive();

		MSG_DATA *msg = (MSG_DATA*) event.msg;
		moduleReceive[msg->meta.type](module, event);
	}
}
