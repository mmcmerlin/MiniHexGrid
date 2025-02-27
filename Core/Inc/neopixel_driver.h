/*
 * NEOPIXEL_driver.h
 *
 *  Created on: Feb 26, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#ifndef INC_NEOPIXEL_DRIVER_H_
#define INC_NEOPIXEL_DRIVER_H_

/*
 * INCLUDES
 */

#include "stm32h5xx_hal.h"

/*
 * DEFINES
 */

#define NEOPIXEL_NUM_LEDS		4

#define NEOPIXEL_TIM			htim2
#define NEOPIXEL_TIM_CHANNEL	TIM_CHANNEL_1

#define NEOPIXEL_HI_VAL			64
#define NEOPIXEL_LO_VAL			32
#define NEOPIXEL_RST_PERIODS	48

#define NEOPIXEL_BITS_PER_LED	24

#define NEOPIXEL_DMA_BUF_LEN	(NEOPIXEL_NUM_LEDS * NEOPIXEL_BITS_PER_LED) + NEOPIXEL_RST_PERIODS

typedef union {
	struct {
		uint8_t b;
		uint8_t r;
		uint8_t g;
	} colour;

	uint32_t data;
} NEOPIXEL_LEDDATARGB;

/*
 * VARIABLES
 */

extern			NEOPIXEL_LEDDATARGB	NEOPIXEL_LED_DATA[NEOPIXEL_NUM_LEDS];
extern			uint8_t				NEOPIXEL_DMA_BUF[NEOPIXEL_DMA_BUF_LEN];
extern volatile	uint8_t				NEOPIXEL_DMA_COMPLETE_FLAG;

/*
 * FUNCTIONS
 */

HAL_StatusTypeDef NEOPIXEL_Init();
HAL_StatusTypeDef NEOPIXEL_Update();
void NEOPIXEL_SetColour(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void NEOPIXEL_Callback();

#endif /* INC_NEOPIXEL_DRIVER_H_ */
