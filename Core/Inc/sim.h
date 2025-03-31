/*
 * hex_modules.h
 *
 *  Created on: Mar 17, 2025
 *      Author: Vikram Barr
 */

#ifndef INC_MODULE_H_
#define INC_MODULE_H_

#include <sim_uart.h>

#include <stm32h5xx_hal.h>
#include <cmsis_os2.h>

enum {
	CLEAR,
	UPDATE,
	REQUEST,
	RESPONSE,
	NUM_TYPES,
} MSG_TYPES;

#define SPECIAL 		0b00
#define GENERATION	0b01
#define LOAD 				0b10
#define LINE	 			0b11

#define ANY					0b00
#define LOW					0b01
#define MEDIUM			0b10
#define HIGH				0b11

typedef union {
	uint8_t val;
	struct {
		uint8_t group  : 2;
		uint8_t volts  : 2;
		uint8_t option : 4;
	};
} MODULE_ID;

typedef struct {
	uint8_t pos : 4;
	uint16_t path;
} MODULE_ADDR;

typedef struct {
	enum { HIDDEN, VIEWABLE, EDITABLE } type;
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

	void* custom;
	size_t custom_size;

	void* common;
	size_t common_size;

	void (*customInit)(void*);
	void (*customReceive[NUM_TYPES])(void*, MSG_EVENT);
	void (*customData[NUM_TYPES])(void*, MSG_EVENT*);
} MODULE;

typedef struct {
	uint8_t type;
	MODULE_ID origin;
	MODULE_ID last;
	MODULE_ADDR address;
} META_DATA;

typedef struct {
	uint8_t mode;
	uint8_t day : 3;
	uint8_t hour : 5;
	uint8_t minute;
	int8_t frequency;
} UPDATE_DATA;

typedef union {
	struct {
		int16_t real;
		int16_t reactive;
	} bus;

	struct {
		uint8_t length;
		uint8_t quality;
		int8_t tap;
	} line;
} REQUEST_DATA;

typedef union {
	struct {
		int8_t voltage;
		int16_t reactive;
	} bus;

	struct {
		int16_t mva;
	} line;
} RESPONSE_DATA;

typedef struct {
	META_DATA meta;
	union {
		UPDATE_DATA update;
		REQUEST_DATA request;
		RESPONSE_DATA response;
	} data;
} MSG_DATA;

void SIM_Init(MODULE* module);

void SIM_Loop(MODULE* module);

#endif /* INC_MODULE_H_ */
