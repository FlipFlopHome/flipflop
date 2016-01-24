/*
 * maxim_onewire.c
 *
 *  Created on: Jan 14, 2016
 *      Author: juliendarrah
 */

#include "maxim_onewire.h"

uint8_t MAX_ONEWIRE_reset(uint8_t gpioNumber)
{
	uint8_t result;
	/* Set output low for 480usec */
	GPIO_OUTPUT_SET(gpioNumber, 0);
	os_delay_us(480);

	/* Release the line and wait 60usec */
	GPIO_OUTPUT_SET(gpioNumber, 1);
	GPIO_DIS_OUTPUT(gpioNumber);
	os_delay_us(60);

	/* Read the feedback and wait 420usec to the end of the timeslot */
	result = (uint8_t)(GPIO_INPUT_GET(gpioNumber));
	os_delay_us(420);

	return result;

}

void MAX_ONEWIRE_WriteBit(uint8_t gpioNumber, uint8_t bit)
{
	/* Set output low for 1usec */
	GPIO_OUTPUT_SET(gpioNumber, 0);
	os_delay_us(1);

	/* To write a logic 1 release the line, otherwise keep the line low for 60usec */
	if( bit >= 0x01 )
	{
		GPIO_OUTPUT_SET(gpioNumber, 1);
		GPIO_DIS_OUTPUT(gpioNumber);
	}

	/* No matter what we keep the timeslot for 60usec */
	os_delay_us(60);
	/* Release the line, Go back to listening mode */
	GPIO_OUTPUT_SET(gpioNumber, 1);
	GPIO_DIS_OUTPUT(gpioNumber);
	os_delay_us(1);
}

uint8_t MAX_ONEWIRE_ReadBit(uint8_t gpioNumber)
{
	uint8_t result;
	/* Set output low for 1usec */
	GPIO_OUTPUT_SET(gpioNumber, 0);
	os_delay_us(1);

	/* Release the line, and wait 15usec */
	GPIO_OUTPUT_SET(gpioNumber, 1);
	GPIO_DIS_OUTPUT(gpioNumber);
	os_delay_us(15);

	/* Read the value on the line */
	result = (uint8_t)(GPIO_INPUT_GET(gpioNumber));

	os_delay_us(45);

	return result;
}

void MAX_ONEWIRE_WriteByte(uint8_t gpioNumber, uint8_t byteValue)
{
	uint8_t count;
	count = 0;

	do
	{
		MAX_ONEWIRE_WriteBit( gpioNumber, (byteValue & (1<<count)) );
		count++;
	}while(count < 8);
}

uint8_t MAX_ONEWIRE_ReadByte(uint8_t gpioNumber)
{
	uint8_t count;
	uint8_t result;
	count = 0;
	result = 0;

	do
	{
		result = result | ( MAX_ONEWIRE_ReadBit(gpioNumber) << count);
		count++;
	}while(count < 8);

	return result;

}

uint16_t MAX_DS18B20_GetTemp(uint8_t gpioNumber)
{
	uint16_t temperature;
	uint16_t Tl, Th;

	if( MAX_ONEWIRE_reset(gpioNumber) == 0x01 )
	{
		/* Sensor is not there */
		return (0x0000);
	}
	MAX_ONEWIRE_WriteByte(gpioNumber, 0xCC);
	MAX_ONEWIRE_WriteByte(gpioNumber, 0x44);

	while(MAX_ONEWIRE_ReadBit(gpioNumber) == 0x00);

	MAX_ONEWIRE_reset(gpioNumber);
	MAX_ONEWIRE_WriteByte(gpioNumber, 0xCC);
	MAX_ONEWIRE_WriteByte(gpioNumber, 0xBE);

	Tl = (uint16_t)(MAX_ONEWIRE_ReadByte(gpioNumber)) & 0x00FF;
	Th = (uint16_t)(MAX_ONEWIRE_ReadByte(gpioNumber)) & 0x00FF;

	/* Force a Stop of the resading sequence */
	MAX_ONEWIRE_reset(gpioNumber);

	temperature = ( Tl | ( ((Th)<<8) & 0xFF00) );

	return temperature;

}


