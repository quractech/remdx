/*
 * hw.h
 *
 *  Created on: May 22, 2021
 *      Author: P70
 */

#ifndef INC_HW_H_
#define INC_HW_H_

#include "main.h"

void displayEfficiency(uint8_t en_disp, Ina226Data_t *ch1Data, Ina226Data_t *ch2Data, float *temp_data);
uint8_t digRead(uint8_t dig_pin);
void digitalWrite(uint8_t dig_pin, uint8_t value);
void restartMCU(void);
void saveUserConfigData(uint16_t page, RegisterData_t *inputBuffer);
void getSavedUserConfigData(uint16_t page, RegisterData_t *outBuffer);
void configHW(SetupData_t *setupData, uint8_t update);
void enablePWM(SetupData_t *setupData);
void streamMeasurements(SetupData_t *setupData, float *temp_data, Ina226Data_t *channel1, Ina226Data_t *channel2);
#endif /* INC_HW_H_ */
