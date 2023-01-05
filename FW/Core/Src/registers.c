/*
 * registers.c
 *
 *  Created on: 25 Dec 2022
 *      Author: P70
 */
#include "registers.h"

SetupData_t newSetupData;
SetupData_t defaultSetupData;
SetupData_t customSetupData;
RegisterNumber_e registerNumber;
SetupDataState_e SetupDataState = loadDefaultSetupData;
RegisterData_t regBuffer[NO_REGISTERS] =
{ 0 };

void registerMap(uint8_t *cmd)
{
	static uint8_t w = 119;		// ascii value of w is 119 in decimal
	static uint8_t r = 114;		// ascii value of r is 114 in decimal
//	printf(cmd);
//	printf("cmd=%s\r\n", cmd);
	registerNumber = (uint8_t) (strtol(&cmd[1], NULL, 16) >> 8);
	guserEnteredData = (uint8_t) (strtol(&cmd[3], NULL, 16));

	// Create an array of function pointers, one for each case in the switch statement
	regHandlerFunc regHandlers[NO_REGISTERS] =
	{ &reg0Handler, &reg1Handler, &reg2Handler, &reg3Handler, &reg4Handler, &reg5Handler, &reg6Handler, &reg7Handler,
			&reg8Handler, &reg9Handler, &reg10Handler, &reg11Handler, &reg12Handler, &reg13Handler, &reg14Handler,
			&reg15Handler, &reg16Handler, &reg17Handler, &reg18Handler, &reg19Handler, &reg20Handler, &reg21Handler,
			&reg22Handler, &reg23Handler, &reg24Handler, &reg25Handler, &reg26Handler, &reg27Handler, &reg28Handler,
			&reg29Handler, &reg30Handler, &reg31Handler };
	if (cmd[0] == w)
	{
		if (registerNumber < 2)
		{
			if (registerNumber == 0)
			{
				regBuffer[registerNumber].value = HW_VERSION;
			}
			else if (registerNumber == 1)
			{
				regBuffer[registerNumber].value = FW_VERSION;
			}
		}
		else
		{
			regBuffer[registerNumber].value = guserEnteredData;
		}
		// Call the function pointer for the register number
		regHandlers[registerNumber](guserEnteredData);
		printf("ACK\r\n");
	}
	else if (cmd[0] == r)
	{
		printf("#R#%02X%02X\r\n", registerNumber, regBuffer[registerNumber].value);
	}
	else
	{
		printf("NAK\r\n");
	}
}

void reg0Handler(void)
{
	newSetupData.setRegister.id0 = HW_VERSION;
	printf("HW VERSION: %0.2f\r\n", (float) (HW_VERSION) / 10);
}

void reg1Handler(void)
{
	newSetupData.setRegister.id0 = FW_VERSION;
	printf("FW VERSION: %0.2f\r\n", (float) (FW_VERSION) / 10);
}

void reg2Handler(void)
{
	newSetupData.setRegister.set_led1 = (regBuffer[registerNumber].value & 0x01);
	newSetupData.setRegister.set_led2 = ((regBuffer[registerNumber].value & 0x02) >> 1);
	newSetupData.setRegister.set_dig_out0 = ((regBuffer[registerNumber].value & 0x04) >> 2);
	newSetupData.setRegister.set_dig_out1 = ((regBuffer[registerNumber].value & 0x08) >> 3);
	;
	newSetupData.getRegister.get_int_in0 = ((regBuffer[registerNumber].value & 0x10) >> 4);
	newSetupData.getRegister.get_int_in1 = ((regBuffer[registerNumber].value & 0x20) >> 5);
	newSetupData.setRegister.en_pwm0 = ((regBuffer[registerNumber].value & 0x40) >> 6);
	newSetupData.setRegister.en_pwm1 = ((regBuffer[registerNumber].value & 0x80) >> 7);
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
	g_HWUpdateFlag = 1;
}

void reg3Handler(void)
{
	newSetupData.setRegister.auto_off_dis = (regBuffer[registerNumber].value & 0x01);
	newSetupData.setRegister.meas_mode = ((regBuffer[registerNumber].value & 0x02) >> 1);
	newSetupData.setRegister.en_disp = ((regBuffer[registerNumber].value & 0x04) >> 2);
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
	g_HWUpdateFlag = 1;
}

void reg4Handler(void)
{
	newSetupData.setRegister.set_over_temp_limit = regBuffer[registerNumber].value;
	if (newSetupData.setRegister.set_over_temp_limit > 127)
	{
		lm75ad_setTosLimit(&hi2c1, LM75AD_ADDRESS, (float) (newSetupData.setRegister.set_over_temp_limit - 256));
		lm75ad_setThystLimit(&hi2c1, LM75AD_ADDRESS, (float) (newSetupData.setRegister.set_over_temp_limit - 256 + 5));
		LOGF("tos", (float ) (newSetupData.setRegister.set_over_temp_limit - 256));

	}
	else
	{
		lm75ad_setTosLimit(&hi2c1, LM75AD_ADDRESS, (float) (newSetupData.setRegister.set_over_temp_limit));
		lm75ad_setThystLimit(&hi2c1, LM75AD_ADDRESS, (float) (newSetupData.setRegister.set_over_temp_limit - 5));
	}
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg5Handler(void)
{
	newSetupData.setRegister.bus_volt_conv_time_ch1 = (regBuffer[registerNumber].value & 0x07);
	newSetupData.setRegister.shunt_volt_conv_time_ch1 = ((regBuffer[registerNumber].value & 0x38) >> 3);
	newSetupData.setRegister.limit_unit_prefix_ch1 = ((regBuffer[registerNumber].value & 0x40) >> 6);
	newSetupData.setRegister.limit_unit_prefix_ch2 = ((regBuffer[registerNumber].value & 0x80) >> 7);
	g_ina226Ch1ConfigChangeFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg6Handler(void)
{
	newSetupData.setRegister.bus_volt_conv_time_ch2 = (regBuffer[registerNumber].value & 0x07);
	newSetupData.setRegister.shunt_volt_conv_time_ch2 = ((regBuffer[registerNumber].value & 0x38) >> 3);
	g_ina226Ch2ConfigChangeFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg7Handler(void)
{
	newSetupData.setRegister.en_int_ch1 = (regBuffer[registerNumber].value & 0x01);
	newSetupData.setRegister.en_int_ch2 = ((regBuffer[registerNumber].value & 0x02) >> 1);
	newSetupData.setRegister.ale_trig_ch1 = ((regBuffer[registerNumber].value & 0x1C) >> 2);
	newSetupData.setRegister.ale_trig_ch2 = ((regBuffer[registerNumber].value & 0xE0) >> 5);

	switch (newSetupData.setRegister.ale_trig_ch1)
	{
		case underCurrentAlert:
			ina226_setUnderCurrentAlert(&hi2c1, CH1_ADDRESS);
			break;
		case overCurrentAlert:
			ina226_setOverCurrentAlert(&hi2c1, CH1_ADDRESS);
			break;
		case underVshuntAlert:
			ina226_setShuntUnderVoltageAlert(&hi2c1, CH1_ADDRESS);
			break;
		case overVshuntAlert:
			ina226_setShuntOverVoltageAlert(&hi2c1, CH1_ADDRESS);
			break;
		case underVbusAlert:
			ina226_setBusUnderVoltageAlert(&hi2c1, CH1_ADDRESS);
			break;
		case overVbusAlert:
			ina226_setBusOverVoltageAlert(&hi2c1, CH1_ADDRESS);
			break;
		case overPowerAlert:
			ina226_setOverPowerAlert(&hi2c1, CH1_ADDRESS);
			break;
	}
	switch (newSetupData.setRegister.ale_trig_ch2)
	{
		case underCurrentAlert:
			ina226_setUnderCurrentAlert(&hi2c1, CH2_ADDRESS);
			break;
		case overCurrentAlert:
			ina226_setOverCurrentAlert(&hi2c1, CH2_ADDRESS);
			break;
		case underVshuntAlert:
			ina226_setShuntUnderVoltageAlert(&hi2c1, CH2_ADDRESS);
			break;
		case overVshuntAlert:
			ina226_setShuntOverVoltageAlert(&hi2c1, CH2_ADDRESS);
			break;
		case underVbusAlert:
			ina226_setBusUnderVoltageAlert(&hi2c1, CH2_ADDRESS);
			break;
		case overVbusAlert:
			ina226_setBusOverVoltageAlert(&hi2c1, CH2_ADDRESS);
			break;
		case overPowerAlert:
			ina226_setOverPowerAlert(&hi2c1, CH2_ADDRESS);
			break;
	}

	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg8Handler(void)
{
	newSetupData.setRegister.limit1_ch1 = regBuffer[registerNumber].value;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg9Handler(void)
{
	newSetupData.setRegister.limit2_ch1 = regBuffer[registerNumber].value;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg10Handler(void)
{
	newSetupData.setRegister.limit1_ch2 = regBuffer[registerNumber].value;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg11Handler(void)
{
	newSetupData.setRegister.limit2_ch2 = regBuffer[registerNumber].value;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg12Handler(void)
{
	newSetupData.setRegister.set_pwm0_duty = (
			regBuffer[registerNumber].value > 100 ? 100 : regBuffer[registerNumber].value);
	run_pwm_out0(newSetupData.setRegister.set_pwm0_duty);
	g_HWUpdateFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg13Handler(void)
{
	newSetupData.setRegister.set_pwm1_duty = (
			regBuffer[registerNumber].value > 100 ? 100 : regBuffer[registerNumber].value);
	run_pwm_out1(newSetupData.setRegister.set_pwm1_duty);
	g_HWUpdateFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg14Handler(void)
{
	newSetupData.setRegister.set_pwm_freq1 = (
			regBuffer[registerNumber].value > 255 ? 255 : regBuffer[registerNumber].value);
	newSetupData.setRegister.set_pwm_freq = (uint16_t) ((float) (newSetupData.setRegister.set_pwm_freq1
			+ (newSetupData.setRegister.set_pwm_freq2 << 8)) * PWMFreqCorrectionFactor);
	g_HWUpdateFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg15Handler(void)
{
	newSetupData.setRegister.set_pwm_freq2 = (
			regBuffer[registerNumber].value > 255 ? 255 : regBuffer[registerNumber].value);
	newSetupData.setRegister.set_pwm_freq = (uint16_t) ((float) (newSetupData.setRegister.set_pwm_freq1
			+ (newSetupData.setRegister.set_pwm_freq2 << 8)) * PWMFreqCorrectionFactor);
	g_HWUpdateFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg16Handler(void)
{
//	newSetupData.getRegister.get_adc0 = (regBuffer[registerNumber].value & 0x01);
	//				newSetupData.getRegister.get_adc1 = ((regBuffer[registerNumber].value & 0x02) >> 1);
	//				newSetupData.getRegister.get_vref_mcu = ((regBuffer[registerNumber].value & 0x04) >> 2);
	//				newSetupData.getRegister.get_temp_amb = ((regBuffer[registerNumber].value & 0x08) >> 3);
	//				newSetupData.getRegister.get_temp_mcu = ((regBuffer[registerNumber].value & 0x10) >> 4);
	newSetupData.getRegister.get_dig_in0 = ((regBuffer[registerNumber].value & 0x20) >> 5);
	newSetupData.getRegister.get_dig_in1 = ((regBuffer[registerNumber].value & 0x40) >> 6);
	//				if (newSetupData.getRegister.get_adc0)
	//				{
	//					printf("adc0=%0.2f\r\n", ina_in0);
	//				}
	//				if (newSetupData.getRegister.get_adc1)
	//				{
	//					printf("adc1=%0.2f\r\n", ina_in1);
	//				}
	//				if (newSetupData.getRegister.get_vref_mcu)
	//				{
	//					printf("vref_mcu=%0.2f\r\n", stm32Vref);
	//				}
	//				if (newSetupData.getRegister.get_temp_amb)
	//				{
	//					printf("temp_amb=%0.2f\r\n", lm75adAmbTemp);
	//				}
	//				if (newSetupData.getRegister.get_temp_mcu)
	//				{
	//					printf("temp_mcu=%0.2f\r\n", stm32Temp);
	//				}
	if (newSetupData.getRegister.get_dig_in0)
	{
		printf("IN_DIG0=%d\r\n", digRead(DIG_IN0));
	}
	if (newSetupData.getRegister.get_dig_in1)
	{
		printf("IN_DIG1=%d\r\n", digRead(DIG_IN1));
	}
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg17Handler(void)
{
	newSetupData.setRegister.restore_default = (regBuffer[registerNumber].value & 0x01);
	newSetupData.setRegister.save_config = ((regBuffer[registerNumber].value & 0x02) >> 1);
	if ((newSetupData.setRegister.save_config == 1) && (newSetupData.setRegister.restore_default == 0))
	{
		EEPROMWriteByte(EEPROMLoadPage, 0, (uint8_t*) (1));
		loadSettingFromBuffer(&newSetupData, regBuffer);
		newSetupData.setRegister.save_config = 0;
		regBuffer[reg17].value = (regBuffer[reg17].value & 0xFC);
		saveUserConfigData(EEPROMCustomPage, regBuffer);
		restartMCU();
	}
	if (newSetupData.setRegister.restore_default == 1)
	{
		newSetupData.setRegister.save_config = 0;
		EEPROMWriteByte(EEPROMLoadPage, 0, (uint8_t*) (0));
		getSavedUserConfigData(EEPROMDefaultPage, regBuffer);
		loadSettingFromBuffer(&newSetupData, regBuffer);
		restartMCU();
	}
	else if ((newSetupData.setRegister.restore_default == 0) && (newSetupData.setRegister.save_config == 0))
	{
		EEPROMWriteByte(EEPROMLoadPage, 0, (uint8_t*) (1));
		loadSettingFromBuffer(&newSetupData, regBuffer);
		restartMCU();
	}

	newSetupData.setRegister.ina226_avg_ch1 = ((regBuffer[registerNumber].value & 0x1C) >> 2);
	newSetupData.setRegister.ina226_mode_ch1 = ((regBuffer[registerNumber].value & 0xE0) >> 5);
	g_ina226Ch1ConfigChangeFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg18Handler(void)
{
	newSetupData.setRegister.restart_device = (regBuffer[registerNumber].value & 0x01);
	newSetupData.setRegister.ina226_avg_ch2 = ((regBuffer[registerNumber].value & 0x1C) >> 2);
	newSetupData.setRegister.ina226_mode_ch2 = ((regBuffer[registerNumber].value & 0xE0) >> 5);

	if (newSetupData.setRegister.restart_device == 1)
	{
		newSetupData.setRegister.restart_device = 0;
		restartMCU();
	}

	g_ina226Ch2ConfigChangeFlag = 1;

	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg19Handler(void)
{
	newSetupData.setRegister.ina226_rsh_pwr_ch1 = (regBuffer[registerNumber].value & 0x07);
	newSetupData.setRegister.ina226_rsh_base_ch1 = ((regBuffer[registerNumber].value & 0xF8) >> 3);
	g_ina226Ch1ConfigChangeFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg20Handler(void)
{
	newSetupData.setRegister.ina226_rsh_pwr_ch2 = (regBuffer[registerNumber].value & 0x07);
	newSetupData.setRegister.ina226_rsh_base_ch2 = ((regBuffer[registerNumber].value & 0xF8) >> 3);
	g_ina226Ch2ConfigChangeFlag = 1;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg21Handler(void)
{
	newSetupData.setRegister.blink_led1 = (regBuffer[registerNumber].value & 0x01);
	newSetupData.setRegister.led1_blink_rate = ((regBuffer[registerNumber].value & 0xFE) >> 1) * 10;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg22Handler(void)
{
	newSetupData.setRegister.blink_led2 = (regBuffer[registerNumber].value & 0x01);
	newSetupData.setRegister.led2_blink_rate = ((regBuffer[registerNumber].value & 0xFE) >> 1) * 10;
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg23Handler(void)
{
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg24Handler(void)
{
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg25Handler(void)
{
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg26Handler(void)
{
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg27Handler(void)
{
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg28Handler(void)
{
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg29Handler(void)
{
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg30Handler(void)
{
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void reg31Handler(void)
{
	if (regBuffer[registerNumber].value == 255)
	{
		restartMCU();
	}
	LOGR("register", registerNumber, regBuffer[registerNumber].value);
}

void defaultSettings(SetupData_t *defaultSetupData)
{
	/*********************START REGISTER 0/1 DAFUALT DATA*********************/
	defaultSetupData->setRegister.id0 = HW_VERSION;
	regBuffer[reg0].value = defaultSetupData->setRegister.id0;

	defaultSetupData->setRegister.id1 = FW_VERSION;
	regBuffer[reg1].value = defaultSetupData->setRegister.id1;
	/**********************END REGISTER 0/1 DAFUALT DATA**********************/

	/*********************START REGISTER 2 DAFUALT DATA*********************/
	defaultSetupData->setRegister.set_led1 = 0;
	regBuffer[reg2].value = defaultSetupData->setRegister.set_led1;

	defaultSetupData->setRegister.set_led2 = 0;
	regBuffer[reg2].value |= (defaultSetupData->setRegister.set_led2 << 1);

	defaultSetupData->setRegister.set_dig_out0 = 0;
	regBuffer[reg2].value |= (defaultSetupData->setRegister.set_dig_out0 << 2);

	defaultSetupData->setRegister.set_dig_out1 = 0;
	regBuffer[reg2].value |= (defaultSetupData->setRegister.set_dig_out1 << 3);

	defaultSetupData->getRegister.get_int_in0 = 0;
	regBuffer[reg2].value |= (defaultSetupData->getRegister.get_int_in0 << 4);

	defaultSetupData->getRegister.get_int_in1 = 0;
	regBuffer[reg2].value |= (defaultSetupData->getRegister.get_int_in1 << 5);

	defaultSetupData->setRegister.en_pwm0 = 0;
	regBuffer[reg2].value |= (defaultSetupData->setRegister.en_pwm0 << 6);

	defaultSetupData->setRegister.en_pwm1 = 0;
	regBuffer[reg2].value |= (defaultSetupData->setRegister.en_pwm1 << 7);
	/**********************END REGISTER 2 DAFUALT DATA**********************/

	/*********************START REGISTER 3 DAFUALT DATA*********************/
	//newSetupData->STREAM_DATA.AUTO_OFF_DIS = 1;	// Means that the automatic switch off feature is disabled
	defaultSetupData->setRegister.auto_off_dis = 1;
	regBuffer[reg3].value = defaultSetupData->setRegister.auto_off_dis;

	defaultSetupData->setRegister.meas_mode = 1;
	regBuffer[reg3].value |= (defaultSetupData->setRegister.meas_mode << 1);

	defaultSetupData->setRegister.en_disp = 1;
	regBuffer[reg3].value |= (defaultSetupData->setRegister.en_disp << 2);

	defaultSetupData->getRegister.get_int_temp = 0;
	regBuffer[reg3].value |= (defaultSetupData->getRegister.get_int_temp << 3);

	/**********************END REGISTER 3 DAFUALT DATA**********************/

//	defaultSetupData->adc0_lower_limit = 0;
//	defaultSetupData->adc0_upper_limit = 4095;
//
//	defaultSetupData->adc1_lower_limit = 0;
//	defaultSetupData->adc1_upper_limit = 4095;
	/*********************START REGISTER 4 DAFUALT DATA*********************/
	defaultSetupData->setRegister.set_over_temp_limit = 50;
	regBuffer[reg4].value = defaultSetupData->setRegister.set_over_temp_limit;
	lm75ad_setTosLimit(&hi2c1, LM75AD_ADDRESS, (float) (defaultSetupData->setRegister.set_over_temp_limit));
	lm75ad_setThystLimit(&hi2c1, LM75AD_ADDRESS, (float) (defaultSetupData->setRegister.set_over_temp_limit - 5));
	/**********************END REGISTER 4 DAFUALT DATA**********************/

	/*********************START REGISTER 5 DAFUALT DATA*********************/
	defaultSetupData->setRegister.bus_volt_conv_time_ch1 = 7;
	regBuffer[reg5].value = defaultSetupData->setRegister.bus_volt_conv_time_ch1;
	ina226_setVBUSCT(&hi2c1, CH1_ADDRESS, defaultSetupData->setRegister.bus_volt_conv_time_ch1);

	defaultSetupData->setRegister.shunt_volt_conv_time_ch1 = 7;
	regBuffer[reg5].value |= (defaultSetupData->setRegister.shunt_volt_conv_time_ch1 << 3);
	ina226_setVSHUNTCT(&hi2c1, CH1_ADDRESS, defaultSetupData->setRegister.shunt_volt_conv_time_ch1);

	defaultSetupData->setRegister.limit_unit_prefix_ch1 = 0;
	regBuffer[reg5].value |= (defaultSetupData->setRegister.limit_unit_prefix_ch1 << 6);

	defaultSetupData->setRegister.limit_unit_prefix_ch2 = 0;
	regBuffer[reg5].value |= (defaultSetupData->setRegister.limit_unit_prefix_ch2 << 7);
	/**********************END REGISTER 5 DAFUALT DATA**********************/

	/*********************START REGISTER 6 DAFUALT DATA*********************/
	defaultSetupData->setRegister.bus_volt_conv_time_ch2 = 7;
	regBuffer[reg6].value = defaultSetupData->setRegister.bus_volt_conv_time_ch2;
	ina226_setVBUSCT(&hi2c1, CH2_ADDRESS, defaultSetupData->setRegister.bus_volt_conv_time_ch2);

	defaultSetupData->setRegister.shunt_volt_conv_time_ch2 = 7;
	ina226_setVSHUNTCT(&hi2c1, CH2_ADDRESS, defaultSetupData->setRegister.shunt_volt_conv_time_ch2);
	regBuffer[reg6].value |= (defaultSetupData->setRegister.shunt_volt_conv_time_ch2 << 3);
	/**********************END REGISTER 6 DAFUALT DATA**********************/

	/*********************START REGISTER 7 DAFUALT DATA*********************/
	defaultSetupData->setRegister.en_int_ch1 = 0;
	regBuffer[reg7].value = defaultSetupData->setRegister.en_int_ch1;

	defaultSetupData->setRegister.en_int_ch2 = 0;
	regBuffer[reg7].value |= (defaultSetupData->setRegister.en_int_ch2 << 1);

	defaultSetupData->setRegister.ale_trig_ch1 = overCurrentAlert;
	regBuffer[reg7].value |= (defaultSetupData->setRegister.ale_trig_ch1 << 2);

	defaultSetupData->setRegister.ale_trig_ch2 = overCurrentAlert;
	regBuffer[reg7].value |= (defaultSetupData->setRegister.ale_trig_ch1 << 5);
	/**********************END REGISTER 7 DAFUALT DATA**********************/

	/*********************START REGISTER 8 DAFUALT DATA*********************/
	defaultSetupData->setRegister.limit1_ch1 = 0;
	regBuffer[reg8].value = defaultSetupData->setRegister.limit1_ch1 = 0;
	/**********************END REGISTER 8 DAFUALT DATA**********************/

	/*********************START REGISTER 9 DAFUALT DATA*********************/
	defaultSetupData->setRegister.limit2_ch1 = 0;
	regBuffer[reg9].value = defaultSetupData->setRegister.limit2_ch1 = 0;
	/**********************END REGISTER 9 DAFUALT DATA**********************/

	/*********************START REGISTER 10 DAFUALT DATA*********************/
	defaultSetupData->setRegister.limit1_ch2 = 0;
	regBuffer[reg10].value = defaultSetupData->setRegister.limit1_ch2 = 0;
	/**********************END REGISTER 10 DAFUALT DATA**********************/

	/*********************START REGISTER 11 DAFUALT DATA*********************/
	defaultSetupData->setRegister.limit2_ch2 = 0;
	regBuffer[reg11].value = defaultSetupData->setRegister.limit2_ch2 = 0;
	/**********************END REGISTER 11 DAFUALT DATA**********************/

	/*********************START REGISTER 12 DAFUALT DATA*********************/
	defaultSetupData->setRegister.set_pwm0_duty = 50;
	regBuffer[reg12].value = defaultSetupData->setRegister.set_pwm0_duty;
	/**********************END REGISTER 12 DAFUALT DATA**********************/

	/*********************START REGISTER 13 DAFUALT DATA*********************/
	defaultSetupData->setRegister.set_pwm1_duty = 25;
	regBuffer[reg13].value = defaultSetupData->setRegister.set_pwm1_duty;
	/**********************END REGISTER 13 DAFUALT DATA**********************/

	/*********************START REGISTER 14 DAFUALT DATA*********************/
	// The default PWM frequency is 1kHz
	defaultSetupData->setRegister.set_pwm_freq1 = 232;
	regBuffer[reg14].value = defaultSetupData->setRegister.set_pwm_freq1;
	/**********************END REGISTER 14 DAFUALT DATA**********************/

	/*********************START REGISTER 15 DAFUALT DATA*********************/
	defaultSetupData->setRegister.set_pwm_freq2 = 3;
	regBuffer[reg15].value = defaultSetupData->setRegister.set_pwm_freq2;
	defaultSetupData->setRegister.set_pwm_freq = (uint16_t) ((float) (defaultSetupData->setRegister.set_pwm_freq1
			+ (defaultSetupData->setRegister.set_pwm_freq2 << 8)) * PWMFreqCorrectionFactor);
	/**********************END REGISTER 15 DAFUALT DATA**********************/

	/*********************START REGISTER 16 DAFUALT DATA*********************/
	// GET DATA from MCU
//	defaultSetupData->getRegister.get_adc0 = 0;
//	regBuffer[reg16].value = defaultSetupData->getRegister.get_adc0;
//	defaultSetupData->getRegister.get_adc1 = 0;
//	regBuffer[reg16].value |= (defaultSetupData->getRegister.get_adc1 << 1);
//	defaultSetupData->getRegister.get_vref_mcu = 0;
//	regBuffer[reg16].value |= (defaultSetupData->getRegister.get_vref_mcu << 2);
//	defaultSetupData->getRegister.get_temp_amb = 0;
//	regBuffer[reg16].value |= (defaultSetupData->getRegister.get_temp_amb << 3);
//
//	defaultSetupData->getRegister.get_temp_mcu = 0;
//	regBuffer[reg16].value |= (defaultSetupData->getRegister.get_temp_mcu << 4);
	defaultSetupData->getRegister.get_dig_in0 = 0;
	regBuffer[reg16].value |= (defaultSetupData->getRegister.get_dig_in0 << 5);

	defaultSetupData->getRegister.get_dig_in1 = 0;
	regBuffer[reg16].value |= (defaultSetupData->getRegister.get_dig_in1 << 6);

//	defaultSetupData->getRegister.get_effiency = 0;
//	regBuffer[reg16].value |= (defaultSetupData->getRegister.get_effiency << 7);
	/**********************END REGISTER 16 DAFUALT DATA**********************/

	/**********************START REGISTER 17 DAFUALT DATA**********************/
	defaultSetupData->setRegister.restore_default = 0;
	regBuffer[reg17].value = defaultSetupData->setRegister.restore_default;

	defaultSetupData->setRegister.save_config = 0;
	regBuffer[reg17].value |= (defaultSetupData->setRegister.save_config << 1);

	defaultSetupData->setRegister.ina226_avg_ch1 = 4;
	regBuffer[reg17].value |= (defaultSetupData->setRegister.ina226_avg_ch1 << 2);
//	ina226_setAVG(&hi2c1, CH1_ADDRESS, defaultSetupData->setRegister.ina226_avg_ch1);
	defaultSetupData->setRegister.ina226_mode_ch1 = 7;
	regBuffer[reg17].value |= (defaultSetupData->setRegister.ina226_mode_ch1 << 5);
//	ina226_setMODE(&hi2c1, CH1_ADDRESS, defaultSetupData->setRegister.ina226_mode_ch1);
	/**********************END REGISTER 17 DAFUALT DATA**********************/

	/**********************START REGISTER 18 DAFUALT DATA**********************/
	defaultSetupData->setRegister.ina226_avg_ch2 = 4;
	regBuffer[reg18].value |= (defaultSetupData->setRegister.ina226_avg_ch2 << 2);
//	ina226_setAVG(&hi2c1, CH2_ADDRESS, defaultSetupData->setRegister.ina226_avg_ch2);
	defaultSetupData->setRegister.ina226_mode_ch2 = 7;
	regBuffer[reg18].value |= (defaultSetupData->setRegister.ina226_mode_ch2 << 5);
//	ina226_setMODE(&hi2c1, CH2_ADDRESS, defaultSetupData->setRegister.ina226_mode_ch2);
	/**********************END REGISTER 18 DAFUALT DATA**********************/

	/**********************START REGISTER 19 DAFUALT DATA**********************/
	defaultSetupData->setRegister.ina226_rsh_base_ch1 = 5;
	regBuffer[reg19].value = defaultSetupData->setRegister.ina226_rsh_base_ch1;

	defaultSetupData->setRegister.ina226_rsh_pwr_ch1 = 0;
	regBuffer[reg19].value |= (defaultSetupData->setRegister.ina226_rsh_pwr_ch1 << 3);
//	ina226_setRSHUNT(&hi2c1, CH1_ADDRESS,
//			((defaultSetupData->setRegister.ina226_rsh_base_ch1 << 3)
//					| (defaultSetupData->setRegister.ina226_rsh_pwr_ch1)));
	/**********************END REGISTER 19 DAFUALT DATA**********************/

	/**********************START REGISTER 20 DAFUALT DATA**********************/
	defaultSetupData->setRegister.ina226_rsh_base_ch2 = 10;
	regBuffer[reg20].value = defaultSetupData->setRegister.ina226_rsh_base_ch2;

	defaultSetupData->setRegister.ina226_rsh_pwr_ch2 = 0;
	regBuffer[reg20].value |= (defaultSetupData->setRegister.ina226_rsh_pwr_ch2 << 3);
//	ina226_setRSHUNT(&hi2c1, CH2_ADDRESS,
//			((defaultSetupData->setRegister.ina226_rsh_base_ch2 << 3)
//					| (defaultSetupData->setRegister.ina226_rsh_pwr_ch2)));

	/**********************END REGISTER 20 DAFUALT DATA**********************/

//	g_HWupdatedFlag = 1;
//	g_registerUpdatedFlag = 1;
//	g_ina226Ch1ConfigChangedFlag = 1;
//	g_ina226Ch2ConfigChangedFlag = 1;
}

void loadSettingFromBuffer(SetupData_t *SetupData, RegisterData_t *inputBuffer)
{
	SetupData->setRegister.id0 = (inputBuffer + reg0)->value;

	SetupData->setRegister.id1 = (inputBuffer + reg1)->value;
	/**********************END REGISTER 0/1 DAFUALT DATA**********************/

	/*********************START REGISTER 2 DAFUALT DATA*********************/
	SetupData->setRegister.set_led1 = ((inputBuffer + reg2)->value & 0x01);

	SetupData->setRegister.set_led2 = ((inputBuffer + reg2)->value & 0x02) >> 1;

	SetupData->setRegister.set_dig_out0 = ((inputBuffer + reg2)->value & 0x04) >> 2;

	SetupData->setRegister.set_dig_out1 = ((inputBuffer + reg2)->value & 0x8) >> 3;

	SetupData->getRegister.get_int_in0 = ((inputBuffer + reg2)->value & 0x10) >> 4;

	SetupData->getRegister.get_int_in1 = ((inputBuffer + reg2)->value & 0x20) >> 5;

	SetupData->setRegister.en_pwm0 = ((inputBuffer + reg2)->value & 0x40) >> 6;

	SetupData->setRegister.en_pwm1 = ((inputBuffer + reg2)->value & 0x80) >> 7;
	/**********************END REGISTER 2 DAFUALT DATA**********************/

	/*********************START REGISTER 3 DAFUALT DATA*********************/
	SetupData->setRegister.auto_off_dis = ((inputBuffer + reg3)->value & 0x01);

	SetupData->setRegister.meas_mode = ((inputBuffer + reg3)->value & 0x02) >> 1;

	SetupData->setRegister.en_disp = ((inputBuffer + reg3)->value & 0x04) >> 2;

	SetupData->getRegister.get_int_temp = ((inputBuffer + reg3)->value & 0x08) >> 3;
	SetupData->getRegister.get_detect_5v = ((inputBuffer + reg3)->value & 0x10) >> 4;
	/**********************END REGISTER 3 DAFUALT DATA**********************/

	//	defaultSetupData->adc0_lower_limit = 0;
	//	defaultSetupData->adc0_upper_limit = 4095;
	//
	//	defaultSetupData->adc1_lower_limit = 0;
	//	defaultSetupData->adc1_upper_limit = 4095;
	/*********************START REGISTER 4 DAFUALT DATA*********************/
	SetupData->setRegister.set_over_temp_limit = (inputBuffer + reg4)->value;
	lm75ad_setTosLimit(&hi2c1, LM75AD_ADDRESS, (float) (SetupData->setRegister.set_over_temp_limit));
	lm75ad_setThystLimit(&hi2c1, LM75AD_ADDRESS, (float) (SetupData->setRegister.set_over_temp_limit - 5));
	/**********************END REGISTER 4 DAFUALT DATA**********************/

	/*********************START REGISTER 5 DAFUALT DATA*********************/
	SetupData->setRegister.bus_volt_conv_time_ch1 = ((inputBuffer + reg5)->value & 0x07);
	ina226_setVBUSCT(&hi2c1, CH1_ADDRESS, SetupData->setRegister.bus_volt_conv_time_ch1);

	SetupData->setRegister.shunt_volt_conv_time_ch1 = ((inputBuffer + reg5)->value & 0x38) >> 3;
	ina226_setVSHUNTCT(&hi2c1, CH1_ADDRESS, SetupData->setRegister.shunt_volt_conv_time_ch1);

	SetupData->setRegister.limit_unit_prefix_ch1 = ((inputBuffer + reg5)->value & 0x40) >> 6;

	SetupData->setRegister.limit_unit_prefix_ch2 = ((inputBuffer + reg5)->value & 0x80) >> 7;
	/**********************END REGISTER 5 DAFUALT DATA**********************/

	/*********************START REGISTER 6 DAFUALT DATA*********************/
	SetupData->setRegister.bus_volt_conv_time_ch2 = ((inputBuffer + reg6)->value & 0x07);
	ina226_setVBUSCT(&hi2c1, CH2_ADDRESS, SetupData->setRegister.bus_volt_conv_time_ch2);

	SetupData->setRegister.shunt_volt_conv_time_ch2 = ((inputBuffer + reg6)->value & 0x38) >> 3;
	ina226_setVSHUNTCT(&hi2c1, CH2_ADDRESS, SetupData->setRegister.shunt_volt_conv_time_ch2);
	/**********************END REGISTER 6 DAFUALT DATA**********************/

	/*********************START REGISTER 7 DAFUALT DATA*********************/
	SetupData->setRegister.en_int_ch1 = ((inputBuffer + reg7)->value & 0x01);

	SetupData->setRegister.en_int_ch2 = ((inputBuffer + reg7)->value & 0x02) >> 1;

	SetupData->setRegister.ale_trig_ch1 = ((inputBuffer + reg7)->value & 0x1C) >> 2;

	SetupData->setRegister.ale_trig_ch2 = ((inputBuffer + reg7)->value & 0xE0) >> 5;
	/**********************END REGISTER 7 DAFUALT DATA**********************/

	/*********************START REGISTER 8 DAFUALT DATA*********************/
	SetupData->setRegister.limit1_ch1 = (inputBuffer + reg8)->value;
	/**********************END REGISTER 8 DAFUALT DATA**********************/

	/*********************START REGISTER 9 DAFUALT DATA*********************/
	SetupData->setRegister.limit2_ch1 = (inputBuffer + reg9)->value;
	/**********************END REGISTER 9 DAFUALT DATA**********************/

	/*********************START REGISTER 10 DAFUALT DATA*********************/
	SetupData->setRegister.limit1_ch2 = (inputBuffer + reg10)->value;
	/**********************END REGISTER 10 DAFUALT DATA**********************/

	/*********************START REGISTER 11 DAFUALT DATA*********************/
	SetupData->setRegister.limit2_ch2 = (inputBuffer + reg11)->value;
	/**********************END REGISTER 11 DAFUALT DATA**********************/

	/*********************START REGISTER 12 DAFUALT DATA*********************/
	SetupData->setRegister.set_pwm0_duty = (inputBuffer + reg12)->value;
	/**********************END REGISTER 12 DAFUALT DATA**********************/

	/*********************START REGISTER 13 DAFUALT DATA*********************/
	SetupData->setRegister.set_pwm1_duty = (inputBuffer + reg13)->value;
	/**********************END REGISTER 13 DAFUALT DATA**********************/

	/*********************START REGISTER 14 DAFUALT DATA*********************/
	// The default PWM frequency is 1kHz
	SetupData->setRegister.set_pwm_freq1 = (inputBuffer + reg14)->value;
	/**********************END REGISTER 14 DAFUALT DATA**********************/

	/*********************START REGISTER 15 DAFUALT DATA*********************/
	SetupData->setRegister.set_pwm_freq2 = (inputBuffer + reg15)->value;
	SetupData->setRegister.set_pwm_freq = (uint16_t) ((float) (SetupData->setRegister.set_pwm_freq1
			+ (SetupData->setRegister.set_pwm_freq2 << 8)) * PWMFreqCorrectionFactor);
	/**********************END REGISTER 15 DAFUALT DATA**********************/

	/*********************START REGISTER 16 DAFUALT DATA*********************/
	// GET DATA from MCU
//	SetupData->getRegister.get_adc0 = ((inputBuffer + reg16)->value & 0x01);
//
//	SetupData->getRegister.get_adc1 = ((inputBuffer + reg16)->value & 0x02) >> 1;
//
//	SetupData->getRegister.get_vref_mcu = ((inputBuffer + reg16)->value & 0x04) >> 2;
//
//	SetupData->getRegister.get_temp_amb = ((inputBuffer + reg16)->value & 0x08) >> 3;
//
//	SetupData->getRegister.get_temp_mcu = ((inputBuffer + reg16)->value & 0x10) >> 4;
	SetupData->getRegister.get_dig_in0 = ((inputBuffer + reg16)->value & 0x20) >> 5;

	SetupData->getRegister.get_dig_in1 = ((inputBuffer + reg16)->value & 0x40) >> 6;

//	SetupData->getRegister.get_effiency = ((inputBuffer + reg16)->value & 0x80) >> 7;
	/**********************END REGISTER 16 DAFUALT DATA**********************/

	/**********************START REGISTER 17 DAFUALT DATA**********************/
	SetupData->setRegister.restore_default = ((inputBuffer + reg17)->value & 0x01);

	SetupData->setRegister.save_config = ((inputBuffer + reg17)->value & 0x02) >> 1;

	SetupData->setRegister.ina226_avg_ch1 = ((inputBuffer + reg17)->value & 0x1C) >> 2;
	ina226_setAVG(&hi2c1, CH1_ADDRESS, SetupData->setRegister.ina226_avg_ch1);
	SetupData->setRegister.ina226_mode_ch1 = ((inputBuffer + reg17)->value & 0xE0) >> 5;
	ina226_setMODE(&hi2c1, CH1_ADDRESS, SetupData->setRegister.ina226_mode_ch1);
	/**********************END REGISTER 17 DAFUALT DATA**********************/

	/**********************START REGISTER 18 DAFUALT DATA**********************/
	SetupData->setRegister.ina226_avg_ch2 = ((inputBuffer + reg18)->value & 0x1C) >> 2;
	ina226_setAVG(&hi2c1, CH2_ADDRESS, SetupData->setRegister.ina226_avg_ch2);
	SetupData->setRegister.ina226_mode_ch2 = ((inputBuffer + reg18)->value & 0xE0) >> 5;
	ina226_setMODE(&hi2c1, CH2_ADDRESS, SetupData->setRegister.ina226_mode_ch2);
	/**********************END REGISTER 18 DAFUALT DATA**********************/

	/**********************START REGISTER 19 DAFUALT DATA**********************/
	SetupData->setRegister.ina226_rsh_pwr_ch1 = ((inputBuffer + reg19)->value & 0x07);
	SetupData->setRegister.ina226_rsh_base_ch1 = ((inputBuffer + reg19)->value & 0xF8) >> 3;
//	ina226_setRSHUNT(&hi2c1, CH1_ADDRESS,
//	((SetupData->setRegister.ina226_rsh_base_ch1 << 3) | (SetupData->setRegister.ina226_rsh_pwr_ch1))
//	);
	/**********************END REGISTER 19 DAFUALT DATA**********************/

	/**********************START REGISTER 20 DAFUALT DATA**********************/
	SetupData->setRegister.ina226_rsh_pwr_ch2 = ((inputBuffer + reg20)->value & 0x07);
	SetupData->setRegister.ina226_rsh_base_ch2 = ((inputBuffer + reg20)->value & 0xF8) >> 3;
//	ina226_setRSHUNT(&hi2c1, CH2_ADDRESS,
//	((SetupData->setRegister.ina226_rsh_base_ch2 << 3) | (SetupData->setRegister.ina226_rsh_pwr_ch2))
//	);
	/**********************END REGISTER 20 DAFUALT DATA**********************/

	/**********************START REGISTER 21 DAFUALT DATA**********************/
	SetupData->setRegister.blink_led1 = ((inputBuffer + reg21)->value & 0x01);
	SetupData->setRegister.led1_blink_rate = (((inputBuffer + reg21)->value & 0xFE) >> 1) * 10;
	/***********************END REGISTER 21 DAFUALT DATA***********************/

	/**********************START REGISTER 22 DAFUALT DATA**********************/
	SetupData->setRegister.blink_led2 = ((inputBuffer + reg22)->value & 0x01);
	SetupData->setRegister.led2_blink_rate = (((inputBuffer + reg22)->value & 0xFE) >> 1) * 10;
	/***********************END REGISTER 22 DAFUALT DATA***********************/

	g_HWUpdateFlag = 1;
}

