/*
 * neopixel_driver.c
 *
 *  Created on: Feb 26, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */
#include "main.h"
#include "stm32h5xx_it.h"

NEOPIXEL_LEDDATARGB	NEOPIXEL_LED_DATA[NEOPIXEL_NUM_LEDS];
uint8_t NEOPIXEL_DMA_BUF[NEOPIXEL_DMA_BUF_LEN];

volatile uint8_t NEOPIXEL_DMA_COMPLETE_FLAG;

HAL_StatusTypeDef NEOPIXEL_Init() {
	HAL_StatusTypeDef halStatus = HAL_TIM_PWN_Init(&NEOPIXEL_TIM);

	for (uint16_t bufIndex = 0; bufIndex < NEOPIXEL_DMA_BUF_LEN; bufIndex++) {
		NEOPIXEL_DMA_BUF[bufIndex] = 0;
	}

	NEOPIXEL_DMA_COMPLETE_FLAG = 1;

	return halStatus;
}

void NEOPIXEL_SetColour(uint8_t index, uint8_t r, uint8_t g, uint8_t b) {
	NEOPIXEL_LED_DATA[index].colour.r = r;
	NEOPIXEL_LED_DATA[index].colour.g = g;
	NEOPIXEL_LED_DATA[index].colour.b = b;
}

HAL_StatusTypeDef NEOPIXEL_Update() {
	if (!NEOPIXEL_DMA_COMPLETE_FLAG) {
		return HAL_BUSY;
	}

	uint16_t bufIndex = 0;

	for (uint8_t ledIndex = 0; ledIndex < NEOPIXEL_NUM_LEDS; ledIndex++) {
		for (uint8_t bitIndex = 0; bitIndex < NEOPIXEL_BITS_PER_LED; bitIndex++) {
			if (NEOPIXEL_LED_DATA[ledIndex].data >> bitIndex & 0x01) {
				NEOPIXEL_DMA_BUF[bufIndex] = NEOPIXEL_HI_VAL;
			} else {
				NEOPIXEL_DMA_BUF[bufIndex] = NEOPIXEL_LO_VAL;
			}
			bufIndex++;
		}
	}

	HAL_StatusTypeDef halStatus = HAL_TIM_PWM_Start_DMA(&NEOPIXEL_TIM, NEOPIXEL_TIM_CHANNEL, (uint32_t *) NEOPIXEL_DMA_BUF, NEOPIXEL_DMA_BUF_LEN);

	if (halStatus == HAL_OK) {
		NEOPIXEL_DMA_COMPLETE_FLAG = 0;
	}

	return halStatus;
}

void NEOPIXEL_Callback() {
	HAL_TIM_PWM_Stop_DMA(&NEOPIXEL_TIM, NEOPIXEL_TIM_CHANNEL);
	NEOPIXEL_DMA_COMPLETE_FLAG = 1;
}
