/*
 * maxim_onewire.h
 *
 *  Created on: Jan 14, 2016
 *      Author: juliendarrah
 */

#ifndef DRIVER_MAXIM_ONEWIRE_H_
#define DRIVER_MAXIM_ONEWIRE_H_

#include "os_type.h"
#include "c_types.h"
#include "eagle_soc.h"
#include "gpio.h"
#include "osapi.h"


uint8_t MAX_ONEWIRE_reset(uint8_t gpioNumber);
void MAX_ONEWIRE_WriteBit(uint8_t gpioNumber, uint8_t bit);
uint8_t MAX_ONEWIRE_ReadBit(uint8_t gpioNumber);
void MAX_ONEWIRE_WriteByte(uint8_t gpioNumber, uint8_t byteValue);
uint8_t MAX_ONEWIRE_ReadByte(uint8_t gpioNumber);
uint16_t MAX_DS18B20_GetTemp(uint8_t gpioNumber);


#endif /* DRIVER_MAXIM_ONEWIRE_H_ */
