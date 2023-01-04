/*
 * led.h
 *
 *  Created on: May 25, 2021
 *      Author: P70
 */

#ifndef INC_LED_H_
#define INC_LED_H_
#include "main.h"

void led1On(void);
void led2On(void);

void led1Off(void);
void led2Off(void);

void led1Blink(uint16_t delay);
void led2Blink(uint16_t delay);

void ledBlinkAll(uint16_t delay);

#endif /* INC_LED_H_ */
