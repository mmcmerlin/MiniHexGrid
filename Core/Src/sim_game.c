/*
 * sim_game.c
 *
 *  Created on: Mar 27, 2025
 *      Author: vikra
 */
#include <sim_game.h>
#include <sim_comms.h>

#define MESSAGE_ACTIONS
#include <sim_actions.h>

void (*onMessage[NUM_TYPES])(uint8_t, MSG_DATA) = {
		[CLEAR] = 		&onClear,
		[UPDATE] = 		&onUpdate,
		[REQUEST] = 	&onRequest,
		[RESPONSE] = 	&onResponse,
};

MSG_DATA (*getMessage[NUM_TYPES])() = {
		[CLEAR] = 		&getClear,
		[UPDATE] = 		&getUpdate,
		[REQUEST] = 	&getRequest,
		[RESPONSE] = 	&getResponse,
};

uint8_t mapped = 0;
uint8_t deactivated = 0;

ADDRESS address = { 0 };

struct nav {
	uint8_t up;
	uint8_t left;
	uint8_t right;
} dirs;

void SIM_HandleMessage(RX_EVENT event) {
	uint8_t port = event.port;
	MSG_DATA msg = event.msg;

	switch (msg.meta.type) {
	case UPDATE:
		break;
	case RESPONSE:
		// immediately relay the message to the master
		SIM_Transmit(self.upstream, copy);

		// check compatibility
		if (SIM_Compatible(self.module, copy.meta.last)) {
			NEOPIXEL_SetColour(port + 1, 0, 255, 0);
		} else {
			NEOPIXEL_SetColour(port + 1, 255, 0, 0);
		}
		break;
	case UPDATE:
		// check if we are the recipient
		if (copy.meta.location) {
			copy.meta.location--;
			uint8_t direction = copy.meta.path & 0x01;
			copy.meta.path >>= 1;

			// relay the copy on the correct port
			SIM_Transmit((self.right * direction) + (self.left * !direction), copy);
		} else {
			// copy the data for ourselves
			self.received = copy.data.update;
		}
		break;
	}
}
