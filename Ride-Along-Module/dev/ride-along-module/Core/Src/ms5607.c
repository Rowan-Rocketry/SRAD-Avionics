#include "ms5607.h"

static const uint8_t CMD_MS5607_MEAS_PRES = MS5607_CONV_PRES | MS5607_OSR_4096;
static const uint8_t CMD_MS5607_READ_ADC = MS5607_READ_ADC;
static const uint8_t CMD_MS5607_RESET = MS5607_RESET;

SPI_HandleTypeDef* spiMS5607;

void configureMS5607(SPI_HandleTypeDef* spi)
{
	disableMS5607();
	spiMS5607 = spi;

	enableMS5607();
	HAL_SPI_Transmit(spiMS5607, &CMD_MS5607_RESET, 1, 100);
	disableMS5607();
}

uint32_t measurePressure()
{
	
	// 1. Send convert command over SPI1
	enableMS5607();
	HAL_SPI_Transmit(spiMS5607, &CMD_MS5607_MEAS_PRES, 1, 100);
	disableMS5607();

	// 2. Wait 9ms for conversion
	HAL_Delay(9);

	// 3. Read ADC result
	return readMS5607ADC();
}

uint32_t readMS5607ADC()
{
	uint8_t responseBuffer[] = {0,0,0};
	
	enableMS5607();
	HAL_SPI_Transmit(spiMS5607, &CMD_MS5607_READ_ADC, 1, 100);
	HAL_SPI_Receive(spiMS5607, (uint8_t *)responseBuffer, 3, 100);
	disableMS5607();

	return ((uint32_t)responseBuffer[0]<<16) | ((uint32_t)responseBuffer[1]<<8) | (uint32_t)responseBuffer[2];
}

void enableMS5607()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}

void disableMS5607()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}
