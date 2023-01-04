/*
 * ina226.c
 *
 *  Created on: Mar 14, 2021
 *      Author: P70
 */

#include "ina226.h"

float ina226_getBusVoltage(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult)
{
	return (ina226_getBusVoltageReg(I2CHandler, Address, preResult) * VBUS_LSB );
}

float ina226_getCurrent(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult)
{
	return (ina226_getCurrentReg(I2CHandler, Address, preResult) * CURRENT_LSB );
}

float ina226_getPower(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult)
{
	return (ina226_getPowerReg(I2CHandler, Address, preResult) * POWER_LSB );
}

float ina226_getShuntVoltage(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult)
{
	return (ina226_getShuntVoltageReg(I2CHandler, Address, preResult) * VSHUNT_LSB );
}

uint8_t ina226_setConfig(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t ConfigWord)
{
	uint8_t sentTable[3];
	sentTable[0] = CONFIG_REG;
	sentTable[1] = (ConfigWord & 0xFF00) >> 8;
	sentTable[2] = (ConfigWord & 0x00FF);
	return HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 3,
	I2CTIMEOUT);
}

uint16_t ina226_getConfig(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	uint8_t sentTable[1] =
	{ CONFIG_REG };
	uint8_t receivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, receivedTable, 2,
	I2CTIMEOUT) != HAL_OK)
		return 0x00;
	else
		return ((uint16_t) receivedTable[0] << 8 | receivedTable[1]);
}

void ina226_setAVG(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t AVG)
{
	uint16_t preConfigData = 0;
	preConfigData = ina226_getConfig(I2CHandler, Address);
	preConfigData = (preConfigData & 0xF1FF);	// Clear bits[11:9]
	preConfigData |= (AVG << 9);				// Shift AVG value into bit field [11:9]
	ina226_setConfig(I2CHandler, Address, preConfigData);
}
void ina226_setMODE(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t MODE)
{
	uint16_t preConfigData = 0;
	preConfigData = ina226_getConfig(I2CHandler, Address);
	preConfigData = (preConfigData & 0xFFF8);	// Clear bits [2:0]
	preConfigData |= MODE;						// Or MODE value into bit field [2:0]
	ina226_setConfig(I2CHandler, Address, preConfigData);
}

void ina226_setVBUSCT(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t VBUSCT)
{
	uint16_t preConfigData = 0;
	preConfigData = ina226_getConfig(I2CHandler, Address);
	preConfigData = (preConfigData & 0xFE3F);	// Clear bits [8:6]
	preConfigData |= (VBUSCT << 6);				// shift VBUS 6 places and or with previous Configuration Data
	ina226_setConfig(I2CHandler, Address, preConfigData);
}

void ina226_setVSHUNTCT(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t VSHUNTCT)
{
	uint16_t preConfigData = 0;
	preConfigData = ina226_getConfig(I2CHandler, Address);
	preConfigData = (preConfigData & 0xFFC7);	// Clear bits [5:3]
	preConfigData |= (VSHUNTCT << 3);			// shift VBUS 3 places and or with previous Configuration Data
	ina226_setConfig(I2CHandler, Address, preConfigData);
}

void ina226_setRSHUNT(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t Rshunt)
{
	/*convRshunt = (Rshunt[7:3]*0.001)*(10^(Rshunt[2:0]))*/
	float convRshunt = (((float) (Rshunt >> 3) * 0.001)) * (pow(10, (float) (Rshunt & 0x07)));
	ina226_setCalibrationReg(I2CHandler, Address, (uint16_t) (INA226_CONST / (convRshunt * CURRENT_LSB )));
}

int32_t ina226_getShuntVoltageReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult)
{
	uint8_t SentTable[1] =
	{ SHUNTV_REG };
	uint8_t ReceivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, SentTable, 1, I2CTIMEOUT);
	HAL_I2C_Master_Receive(I2CHandler, Address, ReceivedTable, 2, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, ReceivedTable, 2,
	I2CTIMEOUT) != HAL_OK)
	{
		return (int32_t) (preResult / VSHUNT_LSB );
	}
	else
	{
		if (ReceivedTable[0] > 127)
		{
			return (int32_t) ((ReceivedTable[0] << 8 | ReceivedTable[1]) - INA226_ADC_RESOLUTION );
		}
		else
		{

			return ((int32_t) ReceivedTable[0] << 8 | ReceivedTable[1]);
		}
	}
}

uint16_t ina226_getBusVoltageReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult)
{
	uint8_t sentTable[1] =
	{ BUSV_REG };
	uint8_t receivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, receivedTable, 2, I2CTIMEOUT) != HAL_OK)
	{
		return (uint16_t) (preResult / VBUS_LSB );
	}
	else
	{
		if (receivedTable[0] > 127)
		{
			return (uint16_t) (INA226_ADC_RESOLUTION - (receivedTable[0] << 8 | receivedTable[1]));
		}
		else
		{
			return ((uint16_t) receivedTable[0] << 8 | receivedTable[1]);
		}
	}
}

uint16_t ina226_getPowerReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult)
{
	uint8_t SentTable[1] =
	{ POWER_REG };
	uint8_t ReceivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, SentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, ReceivedTable, 2,
	I2CTIMEOUT) != HAL_OK)
	{
		return preResult / POWER_LSB ;
	}
	else
		return (uint16_t) (ReceivedTable[0] << 8 | ReceivedTable[1]);

}

int32_t ina226_getCurrentReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult)
{
	uint8_t SentTable[1] =
	{ CURRENT_REG };
	uint8_t ReceivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, SentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, ReceivedTable, 2,
	I2CTIMEOUT) != HAL_OK)
	{
		return (uint16_t) (preResult / CURRENT_LSB );
	}
	else
	{
		if (ReceivedTable[0] > 127)
		{
			return ((int32_t) ((ReceivedTable[0] << 8 | ReceivedTable[1]) - INA226_ADC_RESOLUTION ));
		}
		else
		{

			return ((int32_t) ReceivedTable[0] << 8 | ReceivedTable[1]);
		}
	}

}

uint8_t ina226_setCalibrationReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t ConfigWord)
{
	uint8_t sentTable[3];
	sentTable[0] = CALIB_REG;
	sentTable[1] = (ConfigWord & 0xFF00) >> 8;
	sentTable[2] = (ConfigWord & 0x00FF);
	return HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 3,
	I2CTIMEOUT);
}

uint16_t ina226_getCalibrationReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	uint8_t sentTable[1] =
	{ CALIB_REG };
	uint8_t receivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, receivedTable, 2, I2CTIMEOUT) != HAL_OK)
		return 0xFF;
	else
		return (uint16_t) ((receivedTable[0] << 8) | receivedTable[1]);
}

Ina226_calRegResult_t ina226_getCalReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	Ina226_calRegResult_t calRegResult;
	uint8_t sentTable[1] =
	{ CALIB_REG };
	uint8_t receivedTable[2] =
	{ 0, 0 };
	HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, receivedTable, 2, I2CTIMEOUT) != HAL_OK)
	{
		calRegResult.calVal = 2048;
	}
	else
	{
		calRegResult.calVal = (uint16_t) ((receivedTable[0] << 8) | receivedTable[1]);
	}
	calRegResult.Rshunt = (float) (INA226_CONST / (calRegResult.calVal * CURRENT_LSB ));
	calRegResult.MAXCurrent = (float) (VSHUNT_MAX / calRegResult.Rshunt);

	return calRegResult;
}

void ina226_getResult(Ina226Data_t *resultdef, I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	resultdef->vbus = ina226_getBusVoltage(I2CHandler, Address, resultdef->vbus);
	osDelay(1);
	resultdef->current = ina226_getCurrent(I2CHandler, Address, resultdef->current);
	osDelay(1);
	resultdef->vshunt = ina226_getShuntVoltage(I2CHandler, Address, resultdef->vshunt);
	osDelay(1);
	resultdef->power = ina226_getPower(I2CHandler, Address, resultdef->power);
	osDelay(1);

	/*To combat negative number on the shunt voltage and the current measurements, when there is not voltage applied to the either of the channels*/
//	if (resultdef->vbus < 1.0)
//	{
//		resultdef->vbus = 0.0;
//		resultdef->vshunt = 0.0;
//		resultdef->current = 0.0;
//		resultdef->power = 0.0;
//	}
//	else
//	{
//		resultdef->current = ina226_getCurrent(I2CHandler, Address);
//		resultdef->vshunt = ina226_getShuntVoltage(I2CHandler, Address);
//		resultdef->power = ina226_getPower(I2CHandler, Address);
//	}
}
void ina226_clearResult(Ina226Data_t *result)
{
	result->current = 0;
	result->power = 0;
	result->vbus = 0;
	result->vshunt = 0;
}
uint16_t ina226_getManufID(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	uint8_t sentTable[1] =
	{ MANUF_ID_REG };
	uint8_t receivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, receivedTable, 2,
	I2CTIMEOUT) != HAL_OK)
		return 0xFF;
	else
		return ((uint16_t) receivedTable[0] << 8 | receivedTable[1]);
}

uint16_t ina226_getDieID(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	uint8_t sentTable[1] =
	{ DIE_ID_REG };
	uint8_t receivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, receivedTable, 2,
	I2CTIMEOUT) != HAL_OK)
		return 0xFF;
	else
		return ((uint16_t) receivedTable[0] << 8 | receivedTable[1]);
}

uint8_t ina226_setMaskEnable(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t ConfigWord)
{
	uint8_t sentTable[3];
	sentTable[0] = MASK_REG;
	sentTable[1] = (ConfigWord & 0xFF00) >> 8;
	sentTable[2] = (ConfigWord & 0x00FF);
	return HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 3,
	I2CTIMEOUT);
}

uint16_t ina226_getMaskEnable(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	uint8_t sentTable[1] =
	{ MASK_REG };
	uint8_t receivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, receivedTable, 2,
	I2CTIMEOUT) != HAL_OK)
		return 0xFF;
	else
		return ((uint16_t) receivedTable[0] << 8 | receivedTable[1]);
}

uint8_t ina226_setAlertLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t ConfigWord)
{
	uint8_t sentTable[3];
	sentTable[0] = ALERTL_REG;
	sentTable[1] = (ConfigWord & 0xFF00) >> 8;
	sentTable[2] = (ConfigWord & 0x00FF);
	return HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 3,
	I2CTIMEOUT);
}

uint16_t ina226_getAlertLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	uint8_t sentTable[1] =
	{ ALERTL_REG };
	uint8_t receivedTable[2];
	HAL_I2C_Master_Transmit(I2CHandler, Address, sentTable, 1, I2CTIMEOUT);
	if (HAL_I2C_Master_Receive(I2CHandler, Address, receivedTable, 2,
	I2CTIMEOUT) != HAL_OK)
		return 0xFF;
	else
		return ((uint16_t) receivedTable[0] << 8 | receivedTable[1]);
}

void ina226_channelInit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t config)
{
	ina226_setCalibrationReg(I2CHandler, Address, CALIB_VAL);
	ina226_setConfig(I2CHandler, Address, config);
}
void ina226_setBusUnderVoltageAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	ina226_setMaskEnable(I2CHandler, Address, (BUL | APOL));
}

void ina226_setBusOverVoltageAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	ina226_setMaskEnable(I2CHandler, Address, (BOL | APOL));
}

void ina226_setBusVoltageLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float limit)
{
	ina226_setAlertLimit(I2CHandler, Address, (uint16_t) (limit / VBUS_LSB ));
}

void ina226_setShuntOverVoltageAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	ina226_setMaskEnable(I2CHandler, Address, (SOL | APOL));
}
void ina226_setShuntUnderVoltageAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	ina226_setMaskEnable(I2CHandler, Address, (SUL | APOL));
}

void ina226_setShuntVoltageLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float limit)
{
	ina226_setAlertLimit(I2CHandler, Address, (uint16_t) (limit / VSHUNT_LSB ));
}

void ina226_setOverPowerAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	ina226_setMaskEnable(I2CHandler, Address, (POL | APOL));
}

void ina226_setPowerLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float limit)
{
	ina226_setAlertLimit(I2CHandler, Address, (uint16_t) (limit / POWER_LSB ));
}

void ina226_setOverCurrentAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	ina226_setMaskEnable(I2CHandler, Address, (SOL | APOL));
}

void ina226_setUnderCurrentAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address)
{
	ina226_setMaskEnable(I2CHandler, Address, (SUL | APOL));
}

void ina226_setCurrentLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float limit)
{
	ina226_setAlertLimit(I2CHandler, Address, (uint16_t) (limit / CURRENT_LSB ));
}

//void ina226_setAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address, AlertTrigger_e alertTrigger, uint16_t limit,
//		uint8_t limit_unit_prefix)
//{
//	float limitToFloat = 0;
//	if (limit_unit_prefix == 0)
//	{
//		limitToFloat = ((float) (limit) / 1000);
//	}
//	else
//	{
//		limitToFloat = (float) (limit);
//	}
//	switch (alertTrigger)
//	{
//		case underCurrentAlert:
//			ina226_setUnderCurrentAlert(I2CHandler, Address);
//			ina226_setCurrentLimit(I2CHandler, Address, limitToFloat);
//			break;
//		case overCurrentAlert:
//			ina226_setOverCurrentAlert(I2CHandler, Address);
//			ina226_setCurrentLimit(I2CHandler, Address, limitToFloat);
//			break;
//		case underVshuntAlert:
//			ina226_setShuntUnderVoltageAlert(I2CHandler, Address);
//			ina226_setShuntVoltageLimit(I2CHandler, Address, limitToFloat);
//			break;
//		case overVshuntAlert:
//			ina226_setShuntOverVoltageAlert(I2CHandler, Address);
//			ina226_setShuntVoltageLimit(I2CHandler, Address, limitToFloat);
//			break;
//		case underVbusAlert:
//			ina226_setBusUnderVoltageAlert(I2CHandler, Address);
//			ina226_setBusVoltageLimit(I2CHandler, Address, limitToFloat);
//			break;
//		case overVbusAlert:
//			ina226_setBusOverVoltageAlert(I2CHandler, Address);
//			ina226_setBusVoltageLimit(I2CHandler, Address, limitToFloat);
//			break;
//		case overPowerAlert:
//			ina226_setOverPowerAlert(I2CHandler, Address);
//			ina226_setPowerLimit(I2CHandler, Address, limitToFloat);
//			break;
//	}
//}
