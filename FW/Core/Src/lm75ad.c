/*
 * lm75ad.c
 *
 *  Created on: Jul 11, 2021
 *      Author: P70
 */

#include "lm75ad.h"

void lm75ad_setConfigReg(I2C_HandleTypeDef *I2CHandler, uint8_t Address, uint8_t configuration)
{
	uint8_t sent_table[2];
	sent_table[0] = CONF_REG;
	sent_table[1] = configuration;
	HAL_I2C_Master_Transmit(I2CHandler, Address, sent_table, 2, I2C_TIMEOUT);
}

uint16_t lm75ad_readTempReg(I2C_HandleTypeDef *I2CHandler, uint8_t Address, float preResult)
{
	uint8_t sent_table[1] =
	{ TEMP_REG };
	uint8_t received_table[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sent_table, 1, I2C_TIMEOUT);

	if (HAL_I2C_Master_Receive(I2CHandler, Address, received_table, 2, I2C_TIMEOUT) != HAL_OK)
	{
		return (uint16_t) (preResult / TEMP_RESOLUTION);
	}
	else
	{
		return (uint16_t) (((received_table[0] << 8) | received_table[1]) >> 5);
	}

}

void lm75ad_setTosLimit(I2C_HandleTypeDef *I2CHandler, uint8_t Address, float tos_limit)
{
	uint16_t sent_limit = 0;
	/*Example: Temp = -55C*/
	/*512-(-55*(-2))*=402 ==> 402<<7=51456 (0x0192<<7 = 0xC900)*/

	if (tos_limit < 0)
	{
		sent_limit = (512 - ((uint16_t) (-tos_limit / TOS_THYST_RESOLUTION))) << 7;
	}
	else
	{
		sent_limit = (uint16_t) (tos_limit / TOS_THYST_RESOLUTION) << 7;
	}

	uint8_t sent_table[3];
	sent_table[0] = TOS_REG;
	sent_table[1] = (sent_limit & 0xFF00) >> 8;
	sent_table[2] = (sent_limit & 0x00FF);
	HAL_I2C_Master_Transmit(I2CHandler, Address, sent_table, 3,
	I2C_TIMEOUT);

}

uint16_t lm75ad_readTosReg(I2C_HandleTypeDef *I2CHandler, uint8_t Address)
{
	uint8_t sent_table[1] =
	{ TOS_REG };
	uint8_t received_table[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sent_table, 1,
	I2C_TIMEOUT);

	HAL_I2C_Master_Receive(I2CHandler, Address, received_table, 2,
	I2C_TIMEOUT);
	return (uint16_t) (((received_table[0] << 8) | received_table[1]) >> 7);

}

float lm75ad_getTosLimit(I2C_HandleTypeDef *I2CHandler, uint8_t Address)
{
	uint16_t data = lm75ad_readTosReg(I2CHandler, Address);
	if ((data >> 8) == 1) // chech if the MSB is 1, meaning a negative temperature is measured
	{
		return (data - 512) * TOS_THYST_RESOLUTION;
	}
	else
	{
		return (data * TOS_THYST_RESOLUTION);

	}
}

void lm75ad_setThystLimit(I2C_HandleTypeDef *I2CHandler, uint8_t Address, float thyst_limit)
{
	uint16_t sent_limit = 0;
	/*Example: Temp = -55C*/
	/*512-(-55*(-2))*=402 ==> 402<<7=51456 (0x0192<<7 = 0xC900)*/

	if (thyst_limit < 0)
	{
		sent_limit = (512 - ((uint16_t) (-thyst_limit / TOS_THYST_RESOLUTION))) << 7;
	}
	else
	{
		sent_limit = (uint16_t) (thyst_limit / TOS_THYST_RESOLUTION) << 7;
	}

	uint8_t sent_table[3];
	sent_table[0] = THYST_REG;
	sent_table[1] = (sent_limit & 0xFF00) >> 8;
	sent_table[2] = (sent_limit & 0x00FF);
	HAL_I2C_Master_Transmit(I2CHandler, Address, sent_table, 3,
	I2C_TIMEOUT);

}

uint16_t lm75ad_readThystReg(I2C_HandleTypeDef *I2CHandler, uint8_t Address)
{
	uint8_t sent_table[1] =
	{ THYST_REG };
	uint8_t received_table[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sent_table, 1,
	I2C_TIMEOUT);

	HAL_I2C_Master_Receive(I2CHandler, Address, received_table, 2,
	I2C_TIMEOUT);
	return (uint16_t) (((received_table[0] << 8) | received_table[1]) >> 7);
}

float lm75ad_getThystLimit(I2C_HandleTypeDef *I2CHandler, uint8_t Address)
{
	uint16_t data = lm75ad_readThystReg(I2CHandler, Address);
	if ((data >> 8) == 1) // chech if the MSB is 1, meaning a negative temperature is measured
	{
		return (data - 512) * TOS_THYST_RESOLUTION;
	}
	else
	{
		return (data * TOS_THYST_RESOLUTION);

	}
}

float lm75ad_getTempValue(I2C_HandleTypeDef *I2CHandler, uint8_t Address, float preResult)
{
	uint16_t data = lm75ad_readTempReg(I2CHandler, Address, preResult);
	/*Example: -25C*/
	/*-25 -> 0b111 1110 0111 (dec 2023) => 2023-2^11(2048)=-25*/
	if ((data >> 10) == 1) // Check if the MSB is 1, meaning a negative temperature is measured
	{
		//data = (~(data - 1)) & 0x07FF; // convert to positive number and keep 11-bits
		return ((float) (data - 2048) * TEMP_RESOLUTION);
	}
	else
	{
		return ((float) (data) * TEMP_RESOLUTION);
	}

}

void lm75ad_getData(I2C_HandleTypeDef *I2CHandler, uint8_t Address, Lm75adData_t *data)
{
	data->temp = lm75ad_getTempValue(I2CHandler, Address, 0.0);
	data->tos = lm75ad_getTosLimit(I2CHandler, Address);
	data->thyst = lm75ad_getThystLimit(I2CHandler, Address);

}

void lm75ad_turnOff(I2C_HandleTypeDef *I2CHandler, uint8_t Address)
{
	uint8_t sent_table[2];
	sent_table[0] = CONF_REG;
	sent_table[1] = SHUTDOWN_HIGH;
	HAL_I2C_Master_Transmit(I2CHandler, Address, sent_table, 2, I2C_TIMEOUT);
}

void lm75ad_turnOn(I2C_HandleTypeDef *I2CHandler, uint8_t Address)
{
	uint8_t sent_table[2];
	sent_table[0] = CONF_REG;
	sent_table[1] = SHUTDOWN_LOW;
	HAL_I2C_Master_Transmit(I2CHandler, Address, sent_table, 2, I2C_TIMEOUT);
}
