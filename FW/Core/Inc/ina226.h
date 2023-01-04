/*
 * ina226.h
 *
 *  Created on: Mar 14, 2021
 *      Author: P70
 */

#ifndef INC_H_
#define INC_H_

//#include "stm32fxxx_hal.h"
#include "main.h"
//#include "stm32f4xx_hal.h"
//#include "uart.h"
#include "math.h"

//#include "global_variables.h"
//#include "registers.h"
//#include "arm_math.h"

#ifndef CH1_ADDRESS
#define CH1_ADDRESS				(0x40<<1)
#endif

#ifndef CH2_ADDRESS
#define CH2_ADDRESS				(0x41<<1)
#endif
// TODO - user specified Rsens, current lsb and Max current
#define INA226_CONST			(float)(0.00512)
#define INA226_ADC_RESOLUTION	(uint32_t)(65536)	// 2^16
#define RSHUNT 					(float)(0.010)
#define VSHUNT_MAX				(float)(81.92e-3)
#define MAX_CURRENT				(float)(VSHUNT_MAX/(RSHUNT))
#define CURRENT_LSB				(float)(2*MAX_CURRENT/INA226_ADC_RESOLUTION) // A/bit 8.4A -> 256uA/bit
//#define CURRENT_LSB			500e-3		// A/bit
//#define CURRENT_LSB				250e-6		// 250uA/bit max current of 8.2A
//#define VSHUNT_LSB				VSHUNT_MAX/32768
#define VSHUNT_LSB				(float)(2.5e-6)
#define VBUS_LSB				(float)(1.25e-3)
#define POWER_LSB				(float)(CURRENT_LSB*25)
#define CALIB_VAL				(uint16_t)(INA226_CONST/(CURRENT_LSB*RSHUNT))

//#define CALIB_VAL				1024
//#define CALIB_VAL				2048
//#define CALIB_VAL				4096

#define I2CTIMEOUT				10

#define CONFIG_REG				0x00 // Configuration Register (R/W)
#define SHUNTV_REG				0x01 // Shunt Voltage (R)
#define BUSV_REG				0x02 // Bus Voltage (R)
#define POWER_REG				0x03 // Power (R)
#define CURRENT_REG				0x04 // Current (R)
#define CALIB_REG				0x05 // Calibration (R/W)
#define MASK_REG				0x06 // Mask/Enable (R/W)
#define ALERTL_REG				0x07 // Alert Limit (R/W)
#define MANUF_ID_REG			0xFE // Manufacturer ID (R)
#define DIE_ID_REG				0xFF // Die ID (R)

#define MODE_POWER_DOWN				(0<<0) // Power-Down
#define MODE_TRIG_SHUNT_VOLTAGE		(1<<0) // Shunt Voltage, Triggered
#define MODE_TRIG_BUS_VOLTAGE		(2<<0) // Bus Voltage, Triggered
#define MODE_TRIG_SHUNT_AND_BUS		(3<<0) // Shunt and Bus, Triggered
#define MODE_POWER_DOWN2			(4<<0) // Power-Down
#define MODE_CONT_SHUNT_VOLTAGE		(5<<0) // Shunt Voltage, Continuous
#define MODE_CONT_BUS_VOLTAGE		(6<<0) // Bus Voltage, Continuous
#define MODE_CONT_SHUNT_AND_BUS		(7<<0) // Shunt and Bus, Continuous

// Shunt Voltage Conversion Time
#define VSH_140uS			(0<<3)
#define VSH_204uS			(1<<3)
#define VSH_332uS			(2<<3)
#define VSH_588uS			(3<<3)
#define VSH_1100uS			(4<<3)
#define VSH_2116uS			(5<<3)
#define VSH_4156uS			(6<<3)
#define VSH_8244uS			(7<<3)

// Bus Voltage Conversion Time (VBUS CT Bit Settings[6-8])
#define VBUS_140uS			(0<<6)
#define VBUS_204uS			(1<<6)
#define VBUS_332uS			(2<<6)
#define VBUS_588uS			(3<<6)
#define VBUS_1100uS			(4<<6)
#define VBUS_2116uS			(5<<6)
#define VBUS_4156uS			(6<<6)
#define VBUS_8244uS			(7<<6)

// Averaging Mode (AVG Bit Settings[9-11])
#define AVG_1				(0<<9)
#define AVG_4				(1<<9)
#define AVG_16				(2<<9)
#define AVG_64				(3<<9)
#define AVG_128				(4<<9)
#define AVG_256				(5<<9)
#define AVG_512				(6<<9)
#define AVG_1024			(7<<9)

// Not used bits [12-14]
#define UNUSED_BITS			(4<<12)

// Reset Bit (RST bit [15])
#define RESET_ACTIVE		(1<<15)
#define RESET_INACTIVE		(0<<15)

// Mask/Enable Register
#define SOL				(1<<15) // Shunt Voltage Over-Voltage
#define SUL				(1<<14) // Shunt Voltage Under-Voltage
#define BOL				(1<<13) // Bus Voltagee Over-Voltage
#define BUL				(1<<12) // Bus Voltage Under-Voltage
#define POL				(1<<11) // Power Over-Limit
#define CNVR			(1<<10) // Conversion Ready
#define AFF				(1<<4)  // Alert Function Flag
#define CVRF			(1<<3)  // Conversion Ready Flag
#define OVF				(1<<2)  // Math Overflow Flag
#define APOL			(1<<1)  // Alert Polarity Bit
#define LEN				(1<<0)  // Alert Latch Enable

#define MANUF_ID_DEFAULT	0x5449
#define DIE_ID_DEFAULT		0x2260

typedef struct
{
	float Rshunt;			// Shunt resistance.
	uint16_t calVal;		// Calibration value.
	float MAXCurrent;		// Maximum current that can be measured.
} Ina226_calRegResult_t;

typedef struct
{
	float current;
	float vbus;
	float vshunt;
	float power;
} Ina226Data_t;

float ina226_getBusVoltage(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult);
float ina226_getCurrent(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult);
float ina226_getPower(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult);
float ina226_getShuntVoltage(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult);

uint8_t ina226_setConfig(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t ConfigWord);
uint16_t ina226_getConfig(I2C_HandleTypeDef *I2CHandler, uint16_t Address);

void ina226_setAVG(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t AVG);
void ina226_setMODE(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t MODE);
void ina226_setVBUSCT(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t VBUSCT);
void ina226_setVSHUNTCT(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t VSHUNTCT);
void ina226_setRSHUNT(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint8_t Rshunt);

int32_t ina226_getShuntVoltageReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult);
uint16_t ina226_getBusVoltageReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult);
uint16_t ina226_getPowerReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult);
int32_t ina226_getCurrentReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float preResult);

uint8_t ina226_setCalibrationReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t ConfigWord);
uint16_t ina226_getCalibrationReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
Ina226_calRegResult_t ina226_getCalReg(I2C_HandleTypeDef *I2CHandler, uint16_t Address);

void ina226_getResult(Ina226Data_t *resultdef, I2C_HandleTypeDef *I2CHandler, uint16_t Address);
void ina226_clearResult(Ina226Data_t *result);

void ina226_channelInit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t config);
uint16_t ina226_getManufID(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
uint16_t ina226_getDieID(I2C_HandleTypeDef *I2CHandler, uint16_t Address);

uint8_t ina226_setMaskEnable(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t ConfigWord);
uint16_t ina226_getMaskEnable(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
uint8_t ina226_setAlertLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, uint16_t ConfigWord);
uint16_t ina226_getAlertLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address);

void ina226_setBusUnderVoltageAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
void ina226_setBusOverVoltageAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address);

void ina226_setBusVoltageLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float limit);

void ina226_setShuntOverVoltageAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
void ina226_setShuntUnderVoltageAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
void ina226_setShuntVoltageLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float limit);

void ina226_setOverPowerAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
void ina226_setPowerLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float limit);

void ina226_setOverCurrentAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
void ina226_setUnderCurrentAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address);
void ina226_setCurrentLimit(I2C_HandleTypeDef *I2CHandler, uint16_t Address, float limit);

//void ina226_setAlert(I2C_HandleTypeDef *I2CHandler, uint16_t Address, AlertTrigger_e alertTrigger, uint16_t limit,
//		uint8_t limit_unit_prefix);

#endif /* INC_H_ */
