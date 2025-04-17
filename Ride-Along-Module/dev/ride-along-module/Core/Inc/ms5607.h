// Prevent recursive inclusion
#ifndef MS5607_H
#define MS5607_H

#include "stm32u5xx_hal.h"

/* MS5607 Barometric Pressure and Temperature Sensor Commands */
#define MS5607_RESET			0x1E
#define MS5607_READ_ADC			0x00
#define MS5607_READ_PROM		0xA0

#define MS5607_CONV_PRES		0x40
#define MS5607_CONV_TEMP		0x50

#define MS5607_OSR_256			0x00
#define MS5607_OSR_512			0x02
#define MS5607_OSR_1024			0x04
#define MS5607_OSR_2048			0x06
#define MS5607_OSR_4096			0x08

typedef uint8_t MS5607_osr_t;

typedef struct {
	SPI_HandleTypeDef* spi;
	TIM_HandleTypeDef* timer;
	GPIO_TypeDef* csPort;
	uint16_t csPin;
	MS5607_osr_t osr;
} MS5607_HandleTypeDef;

typedef struct {
	uint16_t presSens;
	uint16_t presOffset;
	uint16_t tcs;
	uint16_t tco;
	uint16_t tRef;
	uint16_t tempSens;
} MS5607_PromData;

typedef struct {
	uint32_t temp;
	uint32_t pres;
} MS5607_RawVal;

typedef struct {
	int32_t temp;
	int32_t pres;
} MS5607_CompVal;

typedef enum {
	MS5607_STARTUP,
	MS5607_PRES_READ,
	MS5607_TEMP_READ,
	MS5607_IDLE
} MS5607_MeasureState;

// Use chip select pin to enable MS5607
void MS5607_enable();

// Use chip select pin to disable MS5607
void MS5607_disable();

// Pulse chip select, set up SPI port, and reset MS5607
void MS5607_init(MS5607_HandleTypeDef* MS5607_initStruct);

// Measure the uncompensated pressure from the MS5607
void MS5607_readUncompPres();

// Measure the uncompensated temperature form the MS5607
void MS5607_readUncompTemp();

// Read the value in the ADC of the MS5607
uint32_t MS5607_readADC();

// Triggered by measurement timer based on OSR
void MS5607_TimerCallback();

// Get the compensated pressure and temperature readings
MS5607_CompVal MS5607_getCompValues(MS5607_RawVal*);

MS5607_RawVal MS5607_getRawValues();

MS5607_MeasureState MS5607_getState();

#endif
