/*
 * global_variables.h
 *
 *  Created on: Dec 18, 2022
 *      Author: P70
 */

#ifndef INC_GLOBAL_VARIABLES_H_
#define INC_GLOBAL_VARIABLES_H_
#include "main.h"
//#include "stdio.h"

#define HW_VERSION 25
#define FW_VERSION  20
#define registerUpdateTime 	100		// Used for the delay time in startRegisterUpdateTask() function
#define USBUpdateTime		100		// Used for the delay time in startUARTTask() function
#define OLEDUpdateTime		500		// Used for the delay time in startOLEDUpadeTask() function
#define UpdateTime			100 	// Used for the delay time in startUpadeTask() function
#define LED1UpdateTime		1000	// Used for the delay time in startLED1Task() function
#define LED2UpdateTime		1000	// Used for the delay time in startLED2Task() function
#define PWMFreqCorrectionFactor	1.012

// CONSTANTS - MCU internal temperature calculation
#define V25 		0.76
#define Avg_Slope 	0.0025
#define STM32_ADC_RESOLUTION	4096
#define VDD						3.277	// SMT32 supply voltage (result from directly reading)
#define VSENSE 		VDD/STM32_ADC_RESOLUTION	// ADC reading
#define INA_IN0_ADC_CHANNEL			0
#define INA_IN1_ADC_CHANNEL			1
#define MCU_TEMP_ADC_CHANNEL		2
#define MCU_INT_REF_ADC_CHANNEL		3

#define MIN_DUTY	0
#define MAX_DUTY	100
#define MIN_FREQ	20
#define MAX_FREQ	50000

//#define DEBUG_
#ifdef DEBUG_
#define LOGS(x) 	printf("%s\r\n",x);
#define LOGI( x, y) printf("%s=%d\r\n",x,y);
#define LOGF( x, y) printf("%s=%0.2f\r\n",x,y);
#define LOGR(x,y,z) printf("%s[%d]=%d\r\n", x,y,z);
#else
#define LOGS(x)
#define LOGI(x,y)
#define LOGF(x,y)
#define LOGR(x,y,z)
#endif

extern uint8_t USBRXDataReady;
extern uint8_t USBRXDataLength;
extern uint8_t *USBRXDataBuffer;
extern uint8_t regMapUpdateFlag;
extern uint8_t guserEnteredData;

extern float lm75adAmbTemp;
extern uint32_t adc_data[4];
extern float stm32Temp;
extern float stm32Vref;
extern float ina_in0;
extern float ina_in1;

extern uint8_t g_HWUpdateFlag;
extern volatile uint8_t g_ina226Ch1ConfigChangeFlag;
extern volatile uint8_t g_ina226Ch2ConfigChangeFlag;
#endif /* INC_GLOBAL_VARIABLES_H_ */
