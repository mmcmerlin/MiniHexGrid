/*
 * comms_protocol.h
 *
 *  Created on: Mar 3, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#ifndef INC_COMMS_PROTOCOL_H_
#define INC_COMMS_PROTOCOL_H_

/*
 * Includes
 */
#include "stm32h5xx_hal.h"

/*
 * Defines
 */
#define COMMS_MSG_LEN		9

typedef struct {
  uint8_t mode;
  int8_t time;
  int8_t frequency;
} __game_call;

typedef struct {
  int16_t p;
  int16_t q;
} __bus_response;

typedef struct {
  uint16_t length;
} __line_response;

typedef struct {
  int16_t tap;
} __trans_response;

typedef struct {
	int16_t capacity;
} __storage_response;

typedef struct {
  int8_t voltage;
  int16_t q;
} __bus_instruction;

typedef struct {
  int16_t mva;
} __line_instruction;

typedef struct {
  int16_t mva;
} __trans_instruction;

typedef struct {
	int16_t capacity;
} __storage_instruction;

typedef struct {
	uint8_t type;
	uint16_t path;
	uint8_t location;
	uint8_t sender;
} COMMS_Metadata;

typedef union {
  __game_call 			game;		// 0, 1

  __bus_response 		bus_r;		// 3
  __line_response 		line_r;		// 5
  __trans_response 		trans_r;	// 7
  __storage_response 	storage_r;	// 9

  __bus_instruction 	bus_i;		// 2
  __line_instruction 	line_i;		// 4
  __trans_instruction 	trans_i;	// 6
  __storage_instruction storage_i;	// 8
} COMMS_Data;

typedef struct {
	COMMS_Metadata meta;
	COMMS_Data data;
} COMMS_Message;

typedef struct {
	UART_HandleTypeDef *huart;
	COMMS_Message message;
} COMMS_Update;

#endif /* INC_COMMS_PROTOCOL_H_ */
