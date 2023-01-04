/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
Ina226Data_t channel1Data;
Ina226Data_t channel2Data;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
//extern uint8_t USBRXDataReady;
//extern uint8_t USBRXDataLength;
//extern uint8_t *USBRXDataBuffer;
/* USER CODE END Variables */
osThreadId LED1TaskHandle;
osThreadId USBTaskHandle;
osThreadId OLEDUpdateTaskHandle;
osThreadId fetchReadingTaskHandle;
osThreadId regUpdateTaskHandle;
osThreadId LED2TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void startLED1Task(void const *argument);
void startUSBTask(void const *argument);
void startOLEDUpdateTask(void const *argument);
void startFetchReadingTask(void const *argument);
void startRegUpdateTask(void const *argument);
void startLED2Task(void const *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
		uint32_t *pulIdleTaskStackSize);

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook(void)
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	 to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
	 task. It is essential that code added to this hook function never attempts
	 to block in any way (for example, call xQueueReceive() with a block time
	 specified, or call vTaskDelay()). If the application makes use of the
	 vTaskDelete() API function (as this demo application does) then it is also
	 important that vApplicationIdleHook() is permitted to return to its calling
	 function, because it is the responsibility of the idle task to clean up
	 memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
__weak void vApplicationTickHook(void)
{
	/* This function will be called by each tick interrupt if
	 configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
	 added here, but the tick hook is called from an interrupt context, so
	 code must not attempt to block, and only the interrupt safe FreeRTOS API
	 functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
	/* Run time stack overflow checking is performed if
	 configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
	 called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
__weak void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	 configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
	 function that will get called if a call to pvPortMalloc() fails.
	 pvPortMalloc() is called internally by the kernel whenever a task, queue,
	 timer or semaphore is created. It is also called by various parts of the
	 demo application. If heap_1.c or heap_2.c are used, then the size of the
	 heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	 FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	 to query the size of free heap space that remains (although it does not
	 provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
		uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* definition and creation of LED1Task */
	osThreadDef(LED1Task, startLED1Task, osPriorityNormal, 0, 128);
	LED1TaskHandle = osThreadCreate(osThread(LED1Task), NULL);

	/* definition and creation of USBTask */
	osThreadDef(USBTask, startUSBTask, osPriorityNormal, 0, 256);
	USBTaskHandle = osThreadCreate(osThread(USBTask), NULL);

	/* definition and creation of OLEDUpdateTask */
	osThreadDef(OLEDUpdateTask, startOLEDUpdateTask, osPriorityLow, 0, 256);
	OLEDUpdateTaskHandle = osThreadCreate(osThread(OLEDUpdateTask), NULL);

	/* definition and creation of fetchReadingTask */
	osThreadDef(fetchReadingTask, startFetchReadingTask, osPriorityHigh, 0, 512);
	fetchReadingTaskHandle = osThreadCreate(osThread(fetchReadingTask), NULL);

	/* definition and creation of regUpdateTask */
	osThreadDef(regUpdateTask, startRegUpdateTask, osPriorityAboveNormal, 0, 256);
	regUpdateTaskHandle = osThreadCreate(osThread(regUpdateTask), NULL);

	/* definition and creation of LED2Task */
	osThreadDef(LED2Task, startLED2Task, osPriorityIdle, 0, 128);
	LED2TaskHandle = osThreadCreate(osThread(LED2Task), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_startLED1Task */
/**
 * @brief  Function implementing the LED1Task thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_startLED1Task */
void startLED1Task(void const *argument)
{
	/* init code for USB_DEVICE */
//	MX_USB_DEVICE_Init();
	/* USER CODE BEGIN startLED1Task */
	/* Infinite loop */
	for (;;)
	{
		if (newSetupData.setRegister.blink_led1 == 1)
		{
			newSetupData.setRegister.set_led1 = 0;
			led1Blink(newSetupData.setRegister.led1_blink_rate);
		}
		else if (newSetupData.setRegister.set_led1 == 1 && newSetupData.setRegister.blink_led1 == 0)
		{
			led1On();
			osDelay(LED1UpdateTime);
		}
		else
		{
			led1Off();
			osDelay(LED1UpdateTime);
		}
	}
	/* USER CODE END startLED1Task */
}

/* USER CODE BEGIN Header_startUSBTask */
/**
 * @brief Function implementing the USBTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startUSBTask */
void startUSBTask(void const *argument)
{
	/* USER CODE BEGIN startUSBTask */
	/* Infinite loop */
	for (;;)
	{
//		printf("regMapUpdateFlag=%d\r\n", regMapUpdateFlag);
		streamMeasurements(&newSetupData, &lm75adAmbTemp, &channel1Data, &channel2Data);
//		printf("id0 = %d\r\n", newSetupData.setRegister.id0);
		osDelay(USBUpdateTime);
	}
	/* USER CODE END startUSBTask */
}

/* USER CODE BEGIN Header_startOLEDUpdateTask */
/**
 * @brief Function implementing the OLEDUpdateTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startOLEDUpdateTask */
void startOLEDUpdateTask(void const *argument)
{
	/* USER CODE BEGIN startOLEDUpdateTask */
	/* Infinite loop */
	for (;;)
	{
		displayEfficiency(1, &channel1Data, &channel2Data, &lm75adAmbTemp);
		osDelay(OLEDUpdateTime);
	}
	/* USER CODE END startOLEDUpdateTask */
}

/* USER CODE BEGIN Header_startFetchReadingTask */
/**
 * @brief Function implementing the fetchReadingTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startFetchReadingTask */
void startFetchReadingTask(void const *argument)
{
	/* USER CODE BEGIN startFetchReadingTask */
	/* Infinite loop */
	for (;;)
	{
		lm75adAmbTemp = lm75ad_getTempValue(&hi2c1, LM75AD_ADDRESS, lm75adAmbTemp);
		ina226_getResult(&channel1Data, &hi2c1, CH1_ADDRESS);
		ina226_getResult(&channel2Data, &hi2c1, CH2_ADDRESS);
		stm32Temp = (VSENSE * adcRead(MCU_TEMP_ADC_CHANNEL) - V25) / Avg_Slope + 25.0;
		stm32Vref = (VSENSE) * adcRead(MCU_INT_REF_ADC_CHANNEL);

		ina_in0 = (VSENSE) * adcRead(INA_IN0_ADC_CHANNEL);

		ina_in1 = VSENSE * adcRead(INA_IN1_ADC_CHANNEL);
//		stm32Temp = (VSENSE * adcRead(2) - V25) / Avg_Slope + 25.0;
//		stm32Vref = (VSENSE) * adcRead(3);
//
//		ina_in0 = (VSENSE) * adcRead(0);
//
//		ina_in1 = VSENSE * adcRead(1);
		osDelay(100);
	}
	/* USER CODE END startFetchReadingTask */
}

/* USER CODE BEGIN Header_startRegUpdateTask */
/**
 * @brief Function implementing the regUpdateTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startRegUpdateTask */
void startRegUpdateTask(void const *argument)
{
	/* USER CODE BEGIN startRegUpdateTask */
	/* Infinite loop */
	for (;;)
	{
		configHW(&newSetupData, g_HWUpdateFlag);
		if (regMapUpdateFlag)
		{
			registerMap(USBRXDataBuffer);
			regMapUpdateFlag = 0;
		}

		osDelay(100);
	}
	/* USER CODE END startRegUpdateTask */
}

/* USER CODE BEGIN Header_startLED2Task */
/**
 * @brief Function implementing the LED2Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startLED2Task */
void startLED2Task(void const *argument)
{
	/* USER CODE BEGIN startLED2Task */
	/* Infinite loop */
	for (;;)
	{
		if (newSetupData.setRegister.blink_led2 == 1)
		{
			newSetupData.setRegister.set_led2 = 0;
//			led2Blink(newSetupData.setRegister.led2_blink_rate);
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
			osDelay(newSetupData.setRegister.led2_blink_rate);
		}
		else if (newSetupData.setRegister.set_led2 == 1 && newSetupData.setRegister.blink_led2 == 0)
		{
			led2On();
			osDelay(LED2UpdateTime);
		}
		else
		{
			led2Off();
			osDelay(LED2UpdateTime);
		}
	}
	/* USER CODE END startLED2Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
