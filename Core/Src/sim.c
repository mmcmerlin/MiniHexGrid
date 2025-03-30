/*
 * hex_modules.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Vikram Barr
 */

#include <sim.h>

#define BASE_ACTIONS
#include <sim_base.h>

#define CUSTOM_ACTIONS
#include <sim_custom.h>

#define MASTER_ACTIONS
#include <sim_master.h>

MODULE modules[] = {
	{
		.display = "Wind",
		.id = {
			.group = GENERATION,
			.volts = MEDIUM,
			.option = 0b0010,
		},
		.customInit = &WIND_Init,
		.customReceive = {
				[UPDATE] = &WIND_onUpdate,
		},
		.customData = {
				[REQUEST] = &WIND_dataRequest,
		},
	},
};

void SIM_Init(MODULE* module) {
	// TODO read pins
	MODULE_ID id = {.val = 0b01100010};
	for (uint8_t i = 0; i < sizeof(modules); i++) {
		if (modules[i].id.val == id.val) {
			*module = modules[i];
			break;
		}
	}

	module->baseInit = &BASE_Init;
	module->baseReceive[CLEAR] = &BASE_onClear;
	module->baseReceive[UPDATE] = &BASE_onUpdate;
	module->baseReceive[REQUEST] = &BASE_onRequest;
	module->baseReceive[RESPONSE] = &BASE_onResponse;
}


}

void SIM_Loop(MODULE* module) {

}
