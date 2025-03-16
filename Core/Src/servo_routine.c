/*
 * servo_routine.c
 *
 *  Created on: Mar 13, 2025
 *      Author: Ikhwan Abd Rahman & Vikram Barr
 */

#include "servo_routine.h"

static TIM_HandleTypeDef *htimX;  // Pointer to the timer instance
static uint32_t PWM_CHANNEL;      // PWM channel for the servo

/**
 * @brief  Initializes the servo by starting the PWM timer.
 * @param  htim: Pointer to the timer handle.
 * @param  channel: PWM channel used for the servo.
 */
void Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel) {
    htimX = htim;
    PWM_CHANNEL = channel;

    // Start PWM
    HAL_TIM_PWM_Start(htimX, PWM_CHANNEL);
}

/**
 * @brief  Sets the servo to a specific angle (Limited between 60° - 120°).
 * @param  angle: Desired servo angle.
 */
void Servo_SetAngle(uint8_t angle) {
    // Constrain angle to the range 60° - 120°
    if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
    if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;

    // Convert angle (60-120) to pulse width (based on full range 0-180 mapping)
    uint32_t pulse_width = SERVO_MIN_PULSE + ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * angle) / 180;

    // Convert pulse width to timer compare value
    uint32_t compare_value = (pulse_width * __HAL_TIM_GET_AUTORELOAD(htimX)) / PWM_PERIOD;

    // Set PWM duty cycle
    __HAL_TIM_SET_COMPARE(htimX, PWM_CHANNEL, compare_value);
}

/**
 * @brief  Updates the servo angle based on a frequency of the grid within 45 - 55 range.
 * @param  host_freq: Frequency of the grid (assumed range: 45 - 55).
 */
void Servo_UpdateAngle(uint16_t host_freq) {
    // Constrain frequency value to the range 45 - 55
    if (host_freq < FREQ_MIN_VALUE) host_freq = FREQ_MIN_VALUE;
    if (host_freq > FREQ_MAX_VALUE) host_freq = FREQ_MAX_VALUE;

    // Map frequency (45 - 55) to servo angle (60 - 120)
    uint8_t angle = SERVO_MIN_ANGLE + ((host_freq - FREQ_MIN_VALUE) * (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE)) /
                    (FREQ_MAX_VALUE - FREQ_MIN_VALUE);

    Servo_SetAngle(angle);
}

/**
 * @brief  Sets the FS90R continuous servo speed.
 * @param  speed: Speed from -100 (full CCW) to 100 (full CW).
 */
void Servo_SetSpeed(int8_t speed) {
    if (speed < -100) speed = -100;
    if (speed > 100) speed = 100;

    uint32_t pulse_width;

    if (speed == 0) {
        pulse_width = FS90R_STOP_PULSE;  // Stop the servo
    } else if (speed > 0) {
        pulse_width = FS90R_STOP_PULSE + ((FS90R_MAX_CW_PULSE - FS90R_STOP_PULSE) * speed) / 100;
    } else {
        pulse_width = FS90R_STOP_PULSE + ((FS90R_MAX_CCW_PULSE - FS90R_STOP_PULSE) * speed) / 100;
    }

    uint32_t compare_value = (pulse_width * __HAL_TIM_GET_AUTORELOAD(htimX)) / PWM_PERIOD;
    __HAL_TIM_SET_COMPARE(htimX, PWM_CHANNEL, compare_value);
}

/**
 * @brief  Stops the FS90R continuous servo.
 */
void Servo_Stop(void) {
    Servo_SetSpeed(0);  // Neutral position (Stop)
}
