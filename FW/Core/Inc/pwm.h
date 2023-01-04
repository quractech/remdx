/*
 * pwm.h
 *
 *  Created on: May 24, 2021
 *      Author: P70
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

//#include "main.h"
#include "tim.h"
//#include "stm32f4xx_hal.h"
//#include "uart.h"
//extern TIM_HandleTypeDef htim4;
void PWMInit(uint16_t freq);
void runPWM(TIM_HandleTypeDef *timer4, uint8_t duty);
void run_pwm_out0(uint8_t duty_cycle);
void stop_pwm_out0(void);
void run_pwm_out1(uint8_t duty_cycle);
void stop_pwm_out1(void);
void set_pwm_freq(uint16_t pwm_freq);

void sweep_pwm_freq(float min_freq, float max_freq, uint16_t no_steps, uint8_t duty_cycle, uint16_t delay);
void sweep_pwm_duty(float min_duty, float max_duty, uint16_t no_steps, uint8_t freq, uint16_t delay);
#endif /* INC_PWM_H_ */
