/*
 * cat24c256.h
 *
 *  Created on: Nov 6, 2021
 *      Author: P70
 */

#ifndef INC_CAT24C256_H_
#define INC_CAT24C256_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdint.h"

// EEPROM ADDRESS (8bits)
#define EEPROM_ADDR (0x50<<1)

// Define the Page Size and number of pages
#define PAGE_SIZE 64     // in Bytes
#define PAGE_NUM  512    // number of pages
typedef struct
{
	int8_t data;
	int16_t page;
} EEPROM_data_t;

void EEPROMWrite(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
void EEPROMRead(uint16_t page, uint16_t offset, uint8_t *data, uint16_t size);
void EEPROMPageErase(uint16_t page);

void EEPROMWriteNum(uint16_t page, uint16_t offset, float fdata);
float EEPROMReadNum(uint16_t page, uint16_t offset);

void float2Bytes(uint8_t *ftoa_bytes_temp, float float_variable);
float bytes2Float(uint8_t *ftoa_bytes_temp);
void EEPROMWriteByte(uint16_t page, uint16_t offset, uint8_t *data);
void EEPROMZeroPage(uint16_t page);
uint8_t EEPROMReadByte(uint16_t page, uint16_t offset);

#endif /* INC_CAT24C256_H_ */
