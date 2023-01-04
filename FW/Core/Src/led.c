/*
 * led.c
 *
 *  Created on: May 25, 2021
 *      Author: P70
 */

#include "led.h"

void led1On(void)
{
	HAL_GPIO_WritePin(GPIOB, LED1_Pin, GPIO_PIN_RESET);
}

void led2On(void)
{
	HAL_GPIO_WritePin(GPIOA, LED2_Pin, GPIO_PIN_RESET);
}

void led1Off(void)
{
	HAL_GPIO_WritePin(GPIOB, LED1_Pin, GPIO_PIN_SET);
}

void led2Off(void)
{
	HAL_GPIO_WritePin(GPIOA, LED2_Pin, GPIO_PIN_SET);
}

void led1Blink(uint16_t delay)
{
	HAL_GPIO_TogglePin(GPIOB, LED1_Pin);
	HAL_Delay((uint16_t) delay);
}

void led2Blink(uint16_t delay)
{
	HAL_GPIO_TogglePin(GPIOA, LED2_Pin);
	HAL_Delay((uint16_t) delay);
}

void ledBlinkAll(uint16_t delay)
{
	HAL_GPIO_TogglePin(GPIOB, LED1_Pin);
	HAL_GPIO_TogglePin(GPIOA, LED2_Pin);
	HAL_Delay(delay);
}
