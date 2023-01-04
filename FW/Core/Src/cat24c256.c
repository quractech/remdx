/*
 * cat24c256.c
 *
 *  Created on: Nov 6, 2021
 *      Author: P70
 */

#include "cat24c256.h"
#include "cmsis_os.h"

/*
 * STEPS when writing data to the eeprom*
 * 1. Pull LOW WP pin
 * 2. Page write
 * 3. write byte to be stored
 * */

extern I2C_HandleTypeDef hi2c1;
#define EEPROM_I2C &hi2c1

/*****************************************************************************************************************************************/
uint8_t bytes_temp[4];

// function to determine the remaining bytes
uint16_t bytestowrite(uint16_t size, uint16_t offset)
{
	if ((size + offset) < PAGE_SIZE)
		return size;
	else
		return PAGE_SIZE - offset;
}

/* write the data to the EEPROM
 * @page is the number of the start page. Range from 0 to PAGE_NUM-1
 * @offset is the start byte offset in the page. Range from 0 to PAGE_SIZE-1
 * @data is the pointer to the data to write in bytes
 * @size is the size of the data
 */
void EEPROMWrite(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size)
{
	HAL_GPIO_WritePin(EEPROM_WP_GPIO_Port, EEPROM_WP_Pin, RESET);
	HAL_Delay(5);
	// Find out the number of bit, where the page addressing starts
	int paddrposition = log(PAGE_SIZE) / log(2);

	// calculate the start page and the end page
	uint16_t startPage = page;
	uint16_t endPage = page + ((size + offset) / PAGE_SIZE);

	// number of pages to be written
	uint16_t numofpages = (endPage - startPage) + 1;
	uint16_t pos = 0;

	// write the data
	for (int i = 0; i < numofpages; i++)
	{
		/* calculate the address of the memory location
		 * Here we add the page address with the byte address
		 */
		uint16_t MemAddress = (startPage << paddrposition) | offset;
		uint16_t bytesremaining = bytestowrite(size, offset); // calculate the remaining bytes to be written

		HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data[pos], bytesremaining, 1000); // write the data to the EEPROM

		startPage += 1; // increment the page, so that a new page address can be selected for further write
		offset = 0;   // since we will be writing to a new page, so offset will be 0
		size = size - bytesremaining;  // reduce the size of the bytes
		pos += bytesremaining;  // update the position for the data buffer

		//HAL_Delay(5);  // Write cycle delay (5ms)
		HAL_Delay(5);
	}
	HAL_GPIO_WritePin(EEPROM_WP_GPIO_Port, EEPROM_WP_Pin, SET);
}

/* READ the data from the EEPROM
 * @page is the number of the start page. Range from 0 to PAGE_NUM-1
 * @offset is the start byte offset in the page. Range from 0 to PAGE_SIZE-1
 * @data is the pointer to the data to write in bytes
 * @size is the size of the data
 */
void EEPROMRead(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size)
{
	int paddrposition = log(PAGE_SIZE) / log(2);

	uint16_t startPage = page;
	uint16_t endPage = page + ((size + offset) / PAGE_SIZE);

	uint16_t numofpages = (endPage - startPage) + 1;
	uint16_t pos = 0;

	for (int i = 0; i < numofpages; i++)
	{
		uint16_t MemAddress = startPage << paddrposition | offset;
		uint16_t bytesremaining = bytestowrite(size, offset);
		HAL_I2C_Mem_Read(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data[pos], bytesremaining, 1000);
		startPage += 1;
		offset = 0;
		size = size - bytesremaining;
		pos += bytesremaining;
	}
}

/*Write the Float/Integer values to the EEPROM
 * @page is the number of the start page. Range from 0 to PAGE_NUM-1
 * @offset is the start byte offset in the page. Range from 0 to PAGE_SIZE-1
 * @data is the float/integer value that you want to write
 */

void EEPROMWriteNum(uint16_t page, uint16_t offset, float data)
{

	float2Bytes(bytes_temp, data);

	EEPROMWrite(page, offset, bytes_temp, 4);
}

/* Reads the single Float/Integer values from the EEPROM
 * @page is the number of the start page. Range from 0 to PAGE_NUM-1
 * @offset is the start byte offset in the page. Range from 0 to PAGE_SIZE-1
 * @returns the float/integer value
 */

float EEPROMReadNum(uint16_t page, uint16_t offset)
{
	uint8_t buffer[4];

	EEPROMRead(page, offset, buffer, 4);

	return (bytes2Float(buffer));
}

/* Erase a page in the EEPROM Memory
 * @page is the number of page to erase
 * In order to erase multiple pages, just use this function in the for loop
 */
void EEPROMPageErase(uint16_t page)
{
	// calculate the memory address based on the page number
	int paddrposition = log(PAGE_SIZE) / log(2);
	uint16_t MemAddress = page << paddrposition;

	// create a buffer to store the reset values
	uint8_t data[PAGE_SIZE];
//	memset(data, 0xff, PAGE_SIZE);
	memset(data, 0x00, PAGE_SIZE);

	// write the data to the EEPROM
	HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, data, PAGE_SIZE, 1000);

	HAL_Delay(5);  // write cycle delay
//	osDelay(5);
}

void float2Bytes(uint8_t *ftoa_bytes_temp, float floatVariable)
{
	union
	{
		float a;
		uint8_t bytes[4];
	} thing;
	thing.bytes[0] = 0;
	thing.bytes[1] = 0;
	thing.bytes[2] = 0;
	thing.bytes[3] = 0;
	thing.a = floatVariable;

	for (uint8_t i = 0; i < 4; i++)
	{
		ftoa_bytes_temp[i] = thing.bytes[i];
	}

}

float bytes2Float(uint8_t *ftoa_bytes_temp)
{
	union
	{
		float a;
		uint8_t bytes[4];
	} thing;

	for (uint8_t i = 0; i < 4; i++)
	{
		thing.bytes[i] = ftoa_bytes_temp[i];
	}

	float floatVariable = thing.a;
	return floatVariable;
}

void EEPROMWriteByte(uint16_t page, uint16_t offset, uint8_t *data)
{
	HAL_GPIO_WritePin(EEPROM_WP_GPIO_Port, EEPROM_WP_Pin, RESET);
	HAL_Delay(1);
	int paddrposition = 6;
	uint16_t startPage = page;
	uint16_t endPage = page;
	uint16_t MemAddress = ((startPage << paddrposition) | offset);

	HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data, 1, 1000);
	HAL_GPIO_WritePin(EEPROM_WP_GPIO_Port, EEPROM_WP_Pin, SET);
	HAL_Delay(1);
}

void EEPROMZeroPage(uint16_t page)
{
	for (uint8_t offset = 0; offset < 256; offset++)
	{
		EEPROMWriteByte(page, offset, 0);
		HAL_Delay(1);
	}
}

uint8_t EEPROMReadByte(uint16_t page, uint16_t offset)
{
	uint8_t *data;
	int paddrposition = 6;
	uint16_t startPage = page;
	uint16_t endPage = page;
	uint16_t MemAddress = ((startPage << paddrposition) | offset);

	HAL_I2C_Mem_Read(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &data, 1, 1000);
	return data;
}
