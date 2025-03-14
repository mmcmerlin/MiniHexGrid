/*
 * servo_routine.h
 *
 *  Created on: Mar 13, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#ifndef INC_SERVO_ROUTINE_H_
#define INC_SERVO_ROUTINE_H_

#include "stm32h5xx_hal.h"

/*Angled servo*/

// Define restricted servo movement range
#define SERVO_MIN_ANGLE 60    // Minimum allowed angle
#define SERVO_MAX_ANGLE 120   // Maximum allowed angle

// Define valid frequency range
#define FREQ_MIN_VALUE 45   // Min frequency value
#define FREQ_MAX_VALUE 55   // Max frequency value

// PWM signal timing
#define SERVO_MIN_PULSE 500   // 0° -> 0.5ms pulse
#define SERVO_MAX_PULSE 2500  // 180° -> 2.5ms pulse
#define PWM_PERIOD 20000      // 20ms period for 50Hz

// Functions
void Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void Servo_SetAngle(uint8_t angle);
void Servo_UpdateAngle(uint16_t sensor_value);

/*Continuous servo*/

#endif /* INC_SERVO_ROUTINE_H_ */
