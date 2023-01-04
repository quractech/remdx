/*
 * pwm.c
 *
 *  Created on: May 24, 2021
 *      Author: P70
 */
#include "pwm.h"

/*#include "pwm.h"
 #include "stm32f1xx_hal_tim.h"
 #include "tim.h"*/
//#include "usart.h"
//TIM_HandleTypeDef htim3;
//extern TIM_HandleTypeDef htim4;
void PWMInit(uint16_t freq)
{
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
	set_pwm_freq(freq);
//	MX_TIM4_Init();

	htim4.Instance->CCR3 = 0;
	htim4.Instance->CCR4 = 0;
	/*HAL_TIM_PWM_Start(timer4, TIM_CHANNEL_3);
	 HAL_TIM_PWM_Start(timer4, TIM_CHANNEL_4);*/

}

void runPWM(TIM_HandleTypeDef *timer4, uint8_t duty)
{
	timer4->Instance->CCR3 = duty;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
}

void run_pwm_out0(uint8_t duty_cycle)
{
	htim4.Instance->CCR3 = duty_cycle;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
}

void stop_pwm_out0(void)
{
	htim4.Instance->CCR3 = 0;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
//	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
}

void run_pwm_out1(uint8_t duty_cycle)
{
	htim4.Instance->CCR4 = duty_cycle;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

void stop_pwm_out1(void)
{
	htim4.Instance->CCR4 = 0;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
//	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
}

void set_pwm_freq(uint16_t pwm_freq)
{
	htim4.Instance->PSC = HAL_RCC_GetSysClockFreq() / (htim4.Instance->ARR * pwm_freq);
}

void sweep_pwm_freq(float min_freq, float max_freq, uint16_t no_steps, uint8_t duty_cycle, uint16_t delay)
{
	float step_size = (max_freq - min_freq) / no_steps;
	float freq = min_freq;
	set_pwm_freq(freq);
	run_pwm_out0(duty_cycle);
	do
	{
		freq += step_size;
		set_pwm_freq(freq);
		run_pwm_out0(duty_cycle);
		HAL_Delay(delay);
		printf("freq = %0.1f\r\n", freq);
	}
	while (freq < max_freq);
	set_pwm_freq(min_freq);
	run_pwm_out0(duty_cycle);
}

void sweep_pwm_duty(float min_duty, float max_duty, uint16_t no_steps, uint8_t freq, uint16_t delay)
{
	float step_size = (max_duty - min_duty) / no_steps;
	set_pwm_freq(freq);
	uint8_t duty = (uint8_t) (min_duty);
	run_pwm_out0(duty);

	do
	{
		duty += step_size;
		run_pwm_out0((uint8_t) (duty));
		HAL_Delay(delay);
	}
	while (duty < max_duty);

}

