/*
 * hex_modules.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Vikram Barr
 */

#include <sim.h>

#include <sim_module.h>
#include <sim_master.h>

#define CUSTOM_ACTIONS
#include <sim_custom.h>

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
}

void SIM_Loop(MODULE* module) {
	if (module->id.val == 0b00001111) {
		MASTER_Main(module);
	} else {
		MODULE_Main(module);
	}
}
