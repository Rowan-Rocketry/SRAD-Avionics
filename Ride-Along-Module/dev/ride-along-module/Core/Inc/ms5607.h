// Prevent recursive inclusion
#ifndef MS5607_H
#define MS5607_H

/* MS5607 Barometric Pressure and Temperature Sensor Commands */
#define MS5607_RESET			0x1E
#define MS5607_READ_ADC			0x00

#define MS5607_CONV_PRES		0x40
#define MS5607_CONV_TEMP		0x50

#define MS5607_OSR_256			0x00
#define MS5607_OSR_512			0x02
#define MS5607_OSR_1024			0x04
#define MS5607_OSR_2048			0x06
#define MS5607_OSR_4096			0x08

#include "stm32u5xx_hal.h"

// Use chip select pin to enable MS5607
void enableMS5607();

// Use chip select pin to disable MS5607
void disableMS5607();

// Pulse chip select, set up SPI port, and reset MS5607
void configureMS5607(SPI_HandleTypeDef*);

// Measure, read, and return the uncompensated pressure from the MS5607
uint32_t measurePressure();

// Read the value in the ADC of the MS5607
uint32_t readMS5607ADC();

#endif
