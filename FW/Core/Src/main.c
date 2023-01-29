/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define CMD_SIZE 3
#define FIRSTTIMEPROG		0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//uint16_t gina226_configData_ch1 = 0;
char transmitBuffer[5];
uint8_t buffer[64];
//extern uint8_t USBRXDataReady;
//extern uint8_t USBRXDataLength;
//extern uint8_t *USBRXDataBuffer;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_TIM4_Init();
	/* USER CODE BEGIN 2 */
	/* init code for USB_DEVICE */
	MX_USB_DEVICE_Init();
//	printf("Initializing...\r\n");
	ssd1306_Init();
	ssd1306_SetCursor(10, 10);
	ssd1306_WriteString("Welcome", Font_11x18, White);
	ssd1306_SetCursor(10, 30);
	ssd1306_WriteString("to REMDx", Font_11x18, White);
	ssd1306_SetCursor(10, 50);
	ssd1306_WriteString("by Qu'rac Tech", Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);

//	ina226_channelInit(&hi2c1, CH1_ADDRESS,
//	RESET_INACTIVE | UNUSED_BITS | AVG_1 | VBUS_1100uS | VSH_1100uS | MODE_CONT_SHUNT_AND_BUS);
	ina226_channelInit(&hi2c1, CH1_ADDRESS,
	RESET_INACTIVE | UNUSED_BITS | AVG_4 | VBUS_140uS | VSH_140uS | MODE_CONT_SHUNT_AND_BUS);
	HAL_Delay(1);
//	ina226_channelInit(&hi2c1, CH2_ADDRESS,
//	RESET_INACTIVE | UNUSED_BITS | AVG_1 | VBUS_1100uS | VSH_1100uS | MODE_CONT_SHUNT_AND_BUS);
	ina226_channelInit(&hi2c1, CH2_ADDRESS,
	RESET_INACTIVE | UNUSED_BITS | AVG_4 | VBUS_140uS | VSH_140uS | MODE_CONT_SHUNT_AND_BUS);
	if (HAL_OK != HAL_ADC_Start_DMA(&hadc1, adc_data, 4))
	{
		Error_Handler();
	}
	CDC_Transmit_FS((uint8_t*) "Starting USB\r\n", sizeof("Starting USB\r\n"));

	SetupDataState = EEPROMReadByte(EEPROMLoadPage, 0);
	if (FIRSTTIMEPROG)
	{
		defaultSettings(&defaultSetupData);
		saveUserConfigData(EEPROMDefaultPage, regBuffer);
		saveUserConfigData(EEPROMCustomPage, regBuffer);
		EEPROMWriteByte(EEPROMLoadPage, 0, (uint8_t*) (1));
		newSetupData = defaultSetupData;
	}
	else
	{
		if (SetupDataState == loadCustomSetupData)
		{
			g_ina226Ch1ConfigUpdateFlag = 1;
			g_ina226Ch2ConfigUpdateFlag = 1;
			getSavedUserConfigData(EEPROMCustomPage, regBuffer);
			loadSettingFromBuffer(&customSetupData, regBuffer);
			newSetupData = customSetupData;
		}
		else
		{
			g_ina226Ch1ConfigUpdateFlag = 1;
			g_ina226Ch2ConfigUpdateFlag = 1;
			getSavedUserConfigData(EEPROMDefaultPage, regBuffer);
			loadSettingFromBuffer(&defaultSetupData, regBuffer);
			//			defaultSettings(&defaultSetupData);
			newSetupData = defaultSetupData;
		}
	}
//	defaultSettings(&defaultSetupData);
	newSetupData = defaultSetupData;
	g_HWUpdateFlag = 1;

	/* USER CODE END 2 */

	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1)
	{
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
