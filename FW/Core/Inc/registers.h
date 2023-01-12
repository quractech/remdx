/*
 * registers.h
 *
 *  Created on: 25 Dec 2022
 *      Author: P70
 */

#ifndef INC_REGISTERS_H_
#define INC_REGISTERS_H_

#include "main.h"

#define NO_REGISTERS 32

typedef enum
{
	reg0 = 0,
	reg1,
	reg2,
	reg3,
	reg4,
	reg5,
	reg6,
	reg7,
	reg8,
	reg9,
	reg10,
	reg11,
	reg12,
	reg13,
	reg14,
	reg15,
	reg16,
	reg17,
	reg18,
	reg19,
	reg20,
	reg21,
	reg22,
	reg23,
	reg24,
	reg25,
	reg26,
	reg27,
	reg28,
	reg29,
	reg30,
	reg31,

} RegisterNumber_e;

typedef struct
{
//	uint8_t data[8];
	uint8_t value;
} RegisterData_t;
typedef enum
{
	loadDefaultSetupData = 0,
	loadCustomSetupData,
} SetupDataState_e;
typedef enum
{
	single_mode = 0,
	stream_mode
} DataOutputMode_e;

typedef enum
{
	underCurrentAlert = 0,
	overCurrentAlert,
	underVshuntAlert,
	overVshuntAlert,
	underVbusAlert,
	overVbusAlert,
	overPowerAlert,
} AlertTrigger_e;

typedef struct
{

	uint8_t get_int_in0; // enable external interrupt 0
	uint8_t get_int_in1; // enable external interrupt 1
	uint8_t get_int_ch1;
	uint8_t get_int_ch2;
	uint8_t get_int_temp;

//	uint8_t get_adc0;
//	uint8_t get_adc1;
//	uint8_t get_vref_mcu;

//	uint8_t get_temp_amb;
//	uint8_t get_temp_mcu;

	uint8_t get_dig_in0;
	uint8_t get_dig_in1;
//	uint8_t get_effiency;

	uint8_t get_detect_5v;

} GetRegister_t;

/*Memory map: the variables in this struct is used to express the configuration data into the device*/
typedef struct
{
	uint8_t id0;
	uint8_t id1;
	uint8_t auto_off_dis;	// disable power saving functionality bit.
	uint8_t meas_mode; // default 0, set this bit to 1 for continues data measurement stream, set this bit to 0 for single data measurement stream.
	uint8_t en_disp; // enable i2c oled display bit. 0 - oled off, 1 - oled display shows current, shunt voltage and bus voltage of both or a single channel.

	uint8_t set_led1;	// set led1: high - (turned on), low - (turned off)
	uint8_t set_led2;	// set led2: high - (turned on), low - (turned off)

	uint8_t blink_led1;			// Enable led1 blink
	uint16_t led1_blink_rate;	// set led1 blink rate in 10 x milliseconds
	uint8_t blink_led2;			// Enable led2 blink
	uint16_t led2_blink_rate;	// set led2 blink rate in 10 x milliseconds

	uint8_t set_dig_out0; // set digital output 0
	uint8_t set_dig_out1; // set digital output 1

	uint8_t en_pwm0; // enable the pwm output 0
	uint8_t en_pwm1; // enable the pwm output 1

	uint8_t set_over_temp_limit;
	uint8_t bus_volt_conv_time_ch1;
	uint8_t shunt_volt_conv_time_ch1;
	uint8_t limit_unit_prefix_ch1;
	uint8_t limit_unit_prefix_ch2;

	uint8_t bus_volt_conv_time_ch2;
	uint8_t shunt_volt_conv_time_ch2;

	uint8_t en_int_ch1; // enable input(channel 1) ina226 alert pin
	uint8_t en_int_ch2; // enable output(channel 2) ina226 alert pin

	AlertTrigger_e ale_trig_ch1;
	AlertTrigger_e ale_trig_ch2;

	// limit_ch1 = limit1_ch1+limit2_ch1<<8;
	uint8_t limit1_ch1; // set input ina226 channel 1 least significant byte
	uint8_t limit2_ch1; // set input ina226 channel 1 most significant byte
	// ch2_limit = ch2_limit1+ch2_limit2<<8;
	uint8_t limit1_ch2; // set output ina226 channel 2 least significant byte
	uint8_t limit2_ch2; // set output ina226 channel 2 most significant byte

	uint8_t set_pwm0_duty; // set duty cycle of the pwm 0
	uint8_t set_pwm1_duty; // set duty cycle of the pwm 1

	/*frequency = set_pwm_freq1+set_pwm__freq2<<8*/
	uint16_t set_pwm_freq;
	uint8_t set_pwm_freq1;	// set pwm frequency least significant byte
	uint8_t set_pwm_freq2;	// set pwm frequency most significant byte

	uint8_t restore_default;
	uint8_t save_config;
	uint8_t ina226_avg_ch1;
	uint8_t ina226_mode_ch1;
	uint8_t ina226_avg_ch2;
	uint8_t ina226_mode_ch2;

	/*Restart device*/
	uint8_t restart_device;

	/*Rshunt_ch1 = (ina226_rsh_base_ch1/1000)*10^(ina226_rsh_pwr_ch1)*/
	uint8_t ina226_rsh_base_ch1;	// Base value of the shunt resistance on channel 1
	uint8_t ina226_rsh_pwr_ch1;		// Power value of the shunt resistance on channel 1
	uint8_t ina226_rsh_base_ch2;	// Base value of the shunt resistance on channel 2
	uint8_t ina226_rsh_pwr_ch2;		// Power value of the shunt resistance on channel 2

} SetRegister_t;

typedef struct
{
	// ADC limit for the ADC watch dog threshold
	uint16_t adc0_lower_limit;
	uint16_t adc0_upper_limit;

	uint16_t adc1_lower_limit;
	uint16_t adc1_upper_limit;

	// temperature limit
	uint16_t temp_limit;

	DataOutputMode_e dataOutputMode;

	// Get data streams
	GetRegister_t getRegister;

	// Set data
	SetRegister_t setRegister;

} SetupData_t;

extern SetupData_t newSetupData;
extern SetupData_t defaultSetupData;
extern SetupData_t customSetupData;
extern RegisterNumber_e registerNumber;
extern SetupDataState_e SetupDataState;
extern RegisterData_t regBuffer[NO_REGISTERS];

// Define a function pointer type
typedef void (*regHandlerFunc)();

void registerMap(uint8_t *cmd);
void reg0Handler(void);
void reg1Handler(void);
void reg2Handler(void);
void reg3Handler(void);
void reg4Handler(void);
void reg5Handler(void);
void reg6Handler(void);
void reg7Handler(void);
void reg8Handler(void);
void reg9Handler(void);
void reg10Handler(void);
void reg11Handler(void);
void reg12Handler(void);
void reg13Handler(void);
void reg14Handler(void);
void reg15Handler(void);
void reg16Handler(void);
void reg17Handler(void);
void reg18Handler(void);
void reg19Handler(void);
void reg20Handler(void);
void reg21Handler(void);
void reg22Handler(void);
void reg23Handler(void);
void reg24Handler(void);
void reg25Handler(void);
void reg26Handler(void);
void reg27Handler(void);
void reg28Handler(void);
void reg29Handler(void);
void reg30Handler(void);
void reg31Handler(void);
void defaultSettings(SetupData_t *defaultSetupData);
void loadSettingFromBuffer(SetupData_t *SetupData, RegisterData_t *inputBuffer);
#endif /* INC_REGISTERS_H_ */
