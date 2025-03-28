/*
 * hex_modules.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Vikram Barr
 */

#include <sim_module.h>

#define MODULE_ACTIONS
#include <sim_actions.h>

MODULE all_modules[] = {
	{
		.id = {
				.group = GENERATOR,
				.volts = MEDIUM,
				.option = 0b0010,
		},
		.display = "Wind",

		.onInit = &WIND_onInit,
		.onResponse = &WIND_onResponse,
		.onUpdate = &WIND_onUpdate,
		.getRequest = &WIND_getRequest,
	},
};
