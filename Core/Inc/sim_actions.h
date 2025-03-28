/*
 * module_behavior.c
 *
 *  Created on: Mar 17, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#ifndef INC_SIM_ACTIONS_H_
#define INC_SIM_ACTIONS_H_

#ifdef MESSAGE_ACTIONS

void onClear(uint8_t port, MSG_DATA msg) {
	if (mapped) {
		COMMS_Transmit(dirs.left, msg);
		COMMS_Transmit(dirs.right, msg);
		mapped = 0;
	}
}

void onUpdate(uint8_t port, MSG_DATA msg) {
	if (!mapped) {
		address = msg.meta.address;

		dirs.up = port;
		dirs.left = (port + 1) % 3;
		dirs.right = (port + 2) % 3;

		mapped = 1;
	}

	if (SIM_Compatible(self.module, copy.meta.last)) {
		self.disabled = 0;
		NEOPIXEL_SetColour(port + 1, 0, 255, 0);
	}
	else {
		self.disabled = 1;
		NEOPIXEL_SetColour(port + 1, 255, 0, 0);
	}

	// relay the message on both ports after editing the address and sender
	copy.meta.last = self.module;
	copy.meta.location++;
	SIM_Transmit(self.left, copy);
	copy.meta.path |= 0x01 << (copy.meta.location - 1);
	SIM_Transmit(self.right, copy);

	// send your local data upstream to the master
	SIM_MESSAGE request;
	request.meta.type = SIM_REQUEST;
	request.meta.path = self.path;
	request.meta.location = self.location;
	request.meta.origin = self.module;
	request.meta.last = self.module;

	response.data.request = self.local;

	if (!self.disabled) {
		SIM_Transmit(self.upstream, request);
	}
}

void onRequest(uint8_t port, MSG_DATA msg) {

}

void onResponse(uint8_t port, MSG_DATA msg) {

}

typedef enum {
    CLEAR,
    UPDATE,
    REQUEST,
    RESPONSE,
		NUM_TYPES
} MSG_TYPES;

#endif /* MESSAGE_ACTIONS */

#ifdef MODULE_ACTIONS
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

void WIND_onInit(void** local, size_t* size, GAME game) {
	*local = (WIND_VARS*) calloc(1, sizeof(WIND_VARS));
	*size = sizeof(WIND_VARS);

	**(WIND_VARS**) local = (WIND_VARS) {
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

void WIND_onUpdate(void* local, GAME game) {
	WIND_VARS* vars = (WIND_VARS*) local;
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

void WIND_onResponse(void* local, RESPONSE_DATA response) {

}

REQUEST_DATA WIND_getRequest(void* local) {

}
#endif /* MODULE_ACTIONS */

#endif /* INC_SIM_ACTIONS_H_ */
