/*
 * global_variables.c
 *
 *  Created on: Dec 25, 2022
 *      Author: P70
 */
#include "global_variables.h"

uint8_t USBRXDataReady = 0;
uint8_t USBRXDataLength = 0;
uint8_t *USBRXDataBuffer;
uint8_t g_regMapUpdateFlag = 0;

uint8_t guserEnteredData;

float lm75adAmbTemp = 0.0;
uint32_t adc_data[4] =
{ 0 };
uint8_t g_HWUpdateFlag = 0;
volatile uint8_t g_ina226Ch1ConfigChangeFlag = 0; // This flag set when the user needs to change the configuration of ina226
volatile uint8_t g_ina226Ch2ConfigChangeFlag = 0; // This flag set when the user needs to change the configuration of ina226

float stm32Temp = 0;
float stm32Vref = 0;
float ina_in0 = 0;
float ina_in1 = 0;
