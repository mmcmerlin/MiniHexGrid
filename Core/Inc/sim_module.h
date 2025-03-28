/*
 * hex_modules.h
 *
 *  Created on: Mar 17, 2025
 *      Author: Vikram Barr
 */

#ifndef INC_SIM_MODULE_H_
#define INC_SIM_MODULE_H_

#include <sim_game.h>
#include <sim_comms.h>

#include <stm32h5xx_hal.h>
#include <cmsis_os2.h>

#define SPECIAL 	0b00
#define GENERATOR 0b01
#define LOAD 			0b10
#define OTHER 		0b11

#define ANY				0b00
#define LOW				0b01
#define MEDIUM		0b10
#define HIGH			0b11

typedef union {
	uint8_t val;
	struct {
		uint8_t group  : 2;
		uint8_t volts  : 2;
		uint8_t option : 4;
	};
} MODULE_ID;

#define HIDDEN	 0
#define VIEWABLE 1
#define EDITABLE 2

typedef struct {
	uint8_t type;
	float val;
	char display[15];
	char unit[4];
	int min;
	int max;
	float step;
} MODULE_VAR;

typedef struct {
	MODULE_ID id;
	char display[15];

	void* vars;
	size_t size;

	void (*onInit)(void**, size_t*, GAME);
	void (*onUpdate)(void*, GAME);
	void (*onResponse)(void*, RESPONSE_DATA);

	REQUEST_DATA (*getRequest)(void*);
} MODULE;

#endif /* INC_SIM_MODULE_H_ */
