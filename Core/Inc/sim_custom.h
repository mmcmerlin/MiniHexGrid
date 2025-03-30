/*
 * module_behavior.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#ifndef INC_MODULE_ACTIONS_H_
#define INC_MODULE_ACTIONS_H_

#include <stdlib.h>

#ifdef CUSTOM_ACTIONS
#define val(a) vars->a.val

typedef struct {
	MODULE_VAR turbines;

	MODULE_VAR real_power;
	MODULE_VAR reactive_power;
	MODULE_VAR wind_speed;

	MODULE_VAR rated_power;
	MODULE_VAR rated_speed;
	MODULE_VAR cut_in;
	MODULE_VAR cut_out;
} WIND_VARS;

void WIND_Init(void* data) {
	MODULE *module = (MODULE*) data;
	module->custom = (WIND_VARS*) calloc(1, sizeof(WIND_VARS));
	module->custom_size = sizeof(WIND_VARS) / sizeof(MODULE_VAR);

	*(WIND_VARS*) module->custom = (WIND_VARS) {
		.turbines = 			{	.type=EDITABLE, .display="# of Turbines", .val=10, .min=1, .max=100, .step=1, .unit=""	},

		.real_power = 		{	.type=VIEWABLE, .display="Real", 				.val=0.0, .unit="MW"		},
		.reactive_power = {	.type=VIEWABLE, .display="Reactive", 		.val=0.0, .unit="MVAr"	},
		.wind_speed = 		{	.type=VIEWABLE, .display="Wind Speed",	.val=0.0, .unit="m/s"		},

		.rated_power = 		{	.type=HIDDEN, .val=7.0,  	.unit="MW"	},
		.rated_speed = 		{	.type=HIDDEN, .val=13.0, 	.unit="m/s"	},
		.cut_in = 				{	.type=HIDDEN, .val=2.0, 	.unit="m/s"	},
		.cut_out = 				{	.type=HIDDEN, .val=20.0, 	.unit="m/s"	},
	};
}

void WIND_onUpdate(void* data, MSG_EVENT update) {
	MODULE *module = (MODULE*) data;
	WIND_VARS* vars = (WIND_VARS*) module->custom;

	float mean = 10.0;
	float std = 6.0;
	float autocorrelation = 0.9;

	// Update wind speed
	float normal_speed = (val(wind_speed) - mean) / std;
	normal_speed = (normal_speed * autocorrelation) + 0; // random number here
	val(wind_speed) = (normal_speed * std) + mean;

	// Update real power
	if (val(wind_speed) < val(cut_in) || val(wind_speed) > val(cut_out)) val(real_power) = 0;
	else if (val(wind_speed) >= val(rated_speed)) val(real_power) = val(rated_power);
	else {
		float slope = val(rated_power) / (val(rated_speed) - val(cut_in));
		val(real_power) = slope * (val(wind_speed) - val(cut_in));
	}
}

void WIND_dataRequest(void* data, MSG_EVENT *request) {
	MODULE *module = (MODULE*) data;
	WIND_VARS* vars = (WIND_VARS*) module->custom;

	MSG_DATA *msg = (MSG_DATA*) request->msg;

	msg->data.request.bus.real = (int16_t) (vars->real_power.val * 10);
	msg->data.request.bus.reactive = (int16_t) (vars->reactive_power.val * 10);
}

#endif /* CUSTOM_ACTIONS */

#endif /* INC_MODULE_ACTIONS_H_ */
