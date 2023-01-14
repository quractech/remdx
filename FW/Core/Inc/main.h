/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "registers.h"
#include "usb_device.h"
#include "global_variables.h"
#include "ssd1306.h"
#include "ssd1306_conf.h"
#include "tim.h"
#include "pwm.h"
#include "cat24c256.h"
#include "ina226.h"
#include "lm75ad.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define EEPROMLoadPage		9
#define EEPROMDefaultPage 	10
#define EEPROMCustomPage 	12
#define DIG_IN0				0
#define DIG_IN1				1
#define DIG_OUT0			0
#define DIG_OUT1			1

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
//extern void USBReceiveHandler(uint8_t *buf, uint32_t *len);
extern uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define INT_OUT_Pin GPIO_PIN_13
#define INT_OUT_GPIO_Port GPIOC
#define INT_OUT_EXTI_IRQn EXTI15_10_IRQn
#define INT_IN_Pin GPIO_PIN_14
#define INT_IN_GPIO_Port GPIOC
#define INT_IN_EXTI_IRQn EXTI15_10_IRQn
#define TEMP_IRQ_Pin GPIO_PIN_15
#define TEMP_IRQ_GPIO_Port GPIOC
#define TEMP_IRQ_EXTI_IRQn EXTI15_10_IRQn
#define LED2_Pin GPIO_PIN_4
#define LED2_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISO_Pin GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define SPI1_MOSI_Pin GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define ANA_IN0_Pin GPIO_PIN_0
#define ANA_IN0_GPIO_Port GPIOB
#define ANA_IN1_Pin GPIO_PIN_1
#define ANA_IN1_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOB
#define DIG_OUT0_Pin GPIO_PIN_12
#define DIG_OUT0_GPIO_Port GPIOB
#define DIG_OUT1_Pin GPIO_PIN_13
#define DIG_OUT1_GPIO_Port GPIOB
#define DIG_IN0_Pin GPIO_PIN_14
#define DIG_IN0_GPIO_Port GPIOB
#define DIG_IN1_Pin GPIO_PIN_15
#define DIG_IN1_GPIO_Port GPIOB
#define SDCard_CS_Pin GPIO_PIN_10
#define SDCard_CS_GPIO_Port GPIOA
#define Auto_off_dis_Pin GPIO_PIN_15
#define Auto_off_dis_GPIO_Port GPIOA
#define INT_IN0_Pin GPIO_PIN_3
#define INT_IN0_GPIO_Port GPIOB
#define INT_IN0_EXTI_IRQn EXTI3_IRQn
#define INT_IN1_Pin GPIO_PIN_4
#define INT_IN1_GPIO_Port GPIOB
#define INT_IN1_EXTI_IRQn EXTI4_IRQn
#define EEPROM_WP_Pin GPIO_PIN_5
#define EEPROM_WP_GPIO_Port GPIOB
#define PWM_OUT0_Pin GPIO_PIN_8
#define PWM_OUT0_GPIO_Port GPIOB
#define PWM_OUT1_Pin GPIO_PIN_9
#define PWM_OUT1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
//extern SetupData_t newSetupData;
//extern SetupData_t defaultSetupData;
//extern SetupData_t customSetupData;
//extern RegisterNumber_e registerNumber;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
