/*
 * lm75ad.h
 *
 *  Created on: Jul 11, 2021
 *      Author: P70
 */

#ifndef INC_LM75AD_H_
#define INC_LM75AD_H_

#include "main.h"
#include "stm32f4xx_hal.h"
//#include "uart.h"

#define LM75AD_ADDRESS (0x48<<1)

#define TEMP_RESOLUTION			0.125
#define TOS_THYST_RESOLUTION	0.5

#define CONF_REG	0x01
#define TEMP_REG	0x00
#define TOS_REG		0x03
#define THYST_REG	0x02

/*Configuration register bit description*/
/*Reserved bist*/
#define CONFIG_UNUSED_BITS		0x00

/*OS fault queue programming*/
#define OS_F_QUE_1				0<<3
#define OS_F_QUE_2				1<<3
#define OS_F_QUE_4				2<<3
#define OS_F_QUE_6				3<<3

/*OS polarity selection*/
#define OS_POL_LOW				0<<2
#define OS_POL_HIGH				1<<2

/*OS operation mode selection*/
#define OS_COMP_INT_LOW			0<<1
#define OS_COMP_INT_HIGH		1<<1

/*Device operation mode selection*/
#define SHUTDOWN_LOW			0
#define SHUTDOWN_HIGH			1

#define I2C_TIMEOUT				10

typedef struct
{
	float temp;
	float tos;
	float thyst;
} Lm75adData_t;

void lm75ad_setConfigReg(I2C_HandleTypeDef *I2CHandler, uint8_t Address, uint8_t configuration);

uint16_t lm75ad_readTempReg(I2C_HandleTypeDef *I2CHandler, uint8_t Address, float preResult);
void lm75ad_setTosLimit(I2C_HandleTypeDef *I2CHandler, uint8_t Address, float tos_limit);
uint16_t lm75ad_readTosReg(I2C_HandleTypeDef *I2CHandler, uint8_t Address);
float lm75ad_getTosLimit(I2C_HandleTypeDef *I2CHandler, uint8_t Address);

void lm75ad_setThystLimit(I2C_HandleTypeDef *I2CHandler, uint8_t Address, float thyst_limit);
uint16_t lm75ad_readThystReg(I2C_HandleTypeDef *I2CHandler, uint8_t Address);

float lm75ad_getThystLimit(I2C_HandleTypeDef *I2CHandler, uint8_t Address);

float lm75ad_getTempValue(I2C_HandleTypeDef *I2CHandler, uint8_t Address, float preResult);

void lm75ad_getData(I2C_HandleTypeDef *I2CHandler, uint8_t Address, Lm75adData_t *data);

void lm75ad_turnOff(I2C_HandleTypeDef *I2CHandler, uint8_t Address);

void lm75ad_turnOn(I2C_HandleTypeDef *I2CHandler, uint8_t Address);

#endif /* INC_LM75AD_H_ */
