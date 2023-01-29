/*
 * hw.c
 *
 *  Created on: May 22, 2021
 *      Author: P70
 */

#include "hw.h"

void displayEfficiency(uint8_t en_disp, Ina226Data_t *ch1Data, Ina226Data_t *ch2Data, float *temp_data)
{
	char str[28];
	if (en_disp == 1)
	{
		ssd1306_SetDisplayOn(1);
		ssd1306_Fill(Black);
		ssd1306_SetCursor(0, 0);
		sprintf(str, "%0.1fC|%0.1f%%", *temp_data, ((ch2Data->power / ch1Data->power) * 100));
		ssd1306_WriteString(str, Font_11x18, White);

		ssd1306_SetCursor(0, 24);
		(ch1Data->vbus >= 1 ? sprintf(str, "%0.3f", ch1Data->vbus) : sprintf(str, "%0.3f", ch1Data->vbus));
		ssd1306_WriteString(str, Font_7x10, White);

		ssd1306_SetCursor(((SSD1306_WIDTH - 10) / 2 - 6), 24);
		sprintf(str, "%0.3f", ch2Data->vbus);
		ssd1306_WriteString(str, Font_7x10, White);

		ssd1306_SetCursor((SSD1306_WIDTH - 20), 24);
		ssd1306_WriteString("V", Font_7x10, White);

		ssd1306_SetCursor(0, 34);
		(ch1Data->current < 1 ? sprintf(str, "%0.1f", ch1Data->current * 1e3) : sprintf(str, "%0.3f", ch1Data->current));
		ssd1306_WriteString(str, Font_7x10, White);

		ssd1306_SetCursor(((SSD1306_WIDTH - 10) / 2 - 6), 34);
		(ch2Data->current < 1 ? sprintf(str, "%0.1f", ch2Data->current * 1e3) : sprintf(str, "%0.3f", ch2Data->current));
		ssd1306_WriteString(str, Font_7x10, White);

		ssd1306_SetCursor((SSD1306_WIDTH - 20), 34);
		(ch1Data->current > 1 && ch2Data->current > 1) ?
				ssd1306_WriteString("A", Font_7x10, White) : ssd1306_WriteString("mA", Font_7x10, White);

		ssd1306_SetCursor(0, 44);
		sprintf(str, "%0.3f", ch1Data->vshunt * 1e3);
		ssd1306_WriteString(str, Font_7x10, White);

		ssd1306_SetCursor(((SSD1306_WIDTH - 10) / 2 - 6), 44);
		sprintf(str, "%0.3f", ch2Data->vshunt * 1e3);
		ssd1306_WriteString(str, Font_7x10, White);

		ssd1306_SetCursor((SSD1306_WIDTH - 20), 44);
		ssd1306_WriteString("mV", Font_7x10, White);

		ssd1306_SetCursor(0, 54);
		sprintf(str, "%0.3f", ch1Data->power);
		ssd1306_WriteString(str, Font_7x10, White);

		ssd1306_SetCursor(((SSD1306_WIDTH - 10) / 2 - 6), 54);
		sprintf(str, "%0.3f", ch2Data->power);
		ssd1306_WriteString(str, Font_7x10, White);

		ssd1306_SetCursor((SSD1306_WIDTH - 20), 54);
		ssd1306_WriteString("W", Font_7x10, White);

		ssd1306_UpdateScreen();
	}
	else
	{
		ssd1306_SetDisplayOn(0);
	}
}

uint8_t digRead(uint8_t dig_pin)
{
	if (dig_pin == 0)
	{
		if (HAL_GPIO_ReadPin(DIG_IN0_GPIO_Port, DIG_IN0_Pin) == GPIO_PIN_RESET)
			return 0;
		else
			return 1;
	}
	else if (dig_pin == 1)
	{
		if (HAL_GPIO_ReadPin(DIG_IN1_GPIO_Port, DIG_IN1_Pin) == GPIO_PIN_RESET)
			return 0;
		else
			return 1;
	}
	else
		return 0;
}

void digitalWrite(uint8_t dig_pin, uint8_t value)
{
	if (dig_pin == 0)
	{
		if (value == 1)
			HAL_GPIO_WritePin(DIG_OUT0_GPIO_Port, DIG_OUT0_Pin, RESET);
		else
			HAL_GPIO_WritePin(DIG_OUT0_GPIO_Port, DIG_OUT0_Pin, SET);
	}
	else if (dig_pin == 1)
	{
		if (value == 1)
			HAL_GPIO_WritePin(DIG_OUT1_GPIO_Port, DIG_OUT1_Pin, RESET);
		else
			HAL_GPIO_WritePin(DIG_OUT1_GPIO_Port, DIG_OUT1_Pin, SET);
	}

}

void restartMCU(void)
{
	HAL_NVIC_SystemReset();
}

void saveUserConfigData(uint16_t page, RegisterData_t *inputBuffer)
{
//	EEPROMPageErase(page);
	for (uint8_t offset = 0; offset < NO_REGISTERS; offset++)
	{
		EEPROMWriteByte(page, offset, (inputBuffer + offset)->value);
		HAL_Delay(1);
	}
}

void getSavedUserConfigData(uint16_t page, RegisterData_t *outBuffer)
{
	for (uint8_t offset = 0; offset < NO_REGISTERS; offset++)
	{
		(outBuffer + offset)->value = EEPROMReadByte(page, offset);
	}

}

void configHW(SetupData_t *setupData, uint8_t update)
{
	if (update == 1)
	{
		digitalWrite(DIG_OUT0, setupData->setRegister.set_dig_out0);
		digitalWrite(DIG_OUT1, setupData->setRegister.set_dig_out1);

//	enableLED(setupData);

		enablePWM(setupData);

//		disablePowerSaveMode(setupData);
		g_HWUpdateFlag = 0;
	}

}

void enablePWM(SetupData_t *setupData)
{
	set_pwm_freq(setupData->setRegister.set_pwm_freq);
	if (setupData->setRegister.en_pwm0)
	{
		run_pwm_out0(setupData->setRegister.set_pwm0_duty);
	}
	else
	{
		stop_pwm_out0();
	}

	if (setupData->setRegister.en_pwm1)
	{
		run_pwm_out1(setupData->setRegister.set_pwm1_duty);
	}
	else
	{
		stop_pwm_out1();
	}
}

void streamMeasurements(SetupData_t *setupData, float *temp_data, Ina226Data_t *channel1, Ina226Data_t *channel2)
{

	if (newSetupData.setRegister.meas_mode == 1)
	{
//		printf(
//				"%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f,%0.3f\r\n",
//				LM75TempReading, stm32Temp, stm32Vref, ina_in0, ina_in1, digRead(DIG_IN0),
//				digRead(DIG_IN1), setupData->setRegister.set_dig_out0, setupData->setRegister.set_dig_out1,
//				setupData->setRegister.en_pwm0, setupData->setRegister.set_pwm0_duty, setupData->setRegister.en_pwm1,
//				setupData->setRegister.set_pwm1_duty, setupData->setRegister.set_pwm_freq, channel1->vshunt,
//				channel1->vbus, channel1->current, channel1->power, channel2->vshunt, channel2->vbus, channel2->current,
//				channel2->power, (channel2->power / channel1->power) * 100);

		printf("#D#%0.1f,%0.1f,%0.3f,%0.6f,%0.3f,%0.6f,%0.3f,%0.6f,%0.3f,%0.6f,%0.3f,%0.3f,%0.3f,%0.3f,%d,%d\r\n",
				*temp_data, stm32Temp, stm32Vref, channel1->vshunt, channel1->vbus, channel1->current, channel1->power,
				channel2->vshunt, channel2->vbus, channel2->current, channel2->power,
				(channel2->power / channel1->power), ina_in0, ina_in1, digRead(DIG_IN0), digRead(DIG_IN1));

//		printf("#D#%0.1f#%0.1f#%0.3f#%0.3f#%0.3f#%0.3f#%0.3f#%0.3f#%0.3f#%0.3f#%0.3f#%0.1f\r\n", LM75TempReading,
//				stm32Temp, stm32Vref, channel1->vshunt * 1e3, channel1->vbus, channel1->current, channel1->power,
//				channel2->vshunt * 1e3, channel2->vbus, channel2->current, channel2->power,
//				(channel2->power / channel1->power) * 100);

//		printf("#D#%0.1f#%0.1f#%0.3f#%0.3f#4#\n", LM75TempReading, stm32Temp, stm32Vref, channel1->vbus);
	}
}
