#include "ms5607.h"

static const uint8_t CMD_MS5607_READ_ADC = MS5607_READ_ADC;
static const uint8_t CMD_MS5607_RESET = MS5607_RESET;

static MS5607_HandleTypeDef* config;
static MS5607_PromData promData;

static MS5607_RawVal rawValues;

static MS5607_MeasureState state;

static uint16_t measurementDelay;
static uint8_t cmdMeasurePres;
static uint8_t cmdMeasureTemp;

void MS5607_config(MS5607_HandleTypeDef* MS5607_initStruct)
{

	config = MS5607_initStruct;
	
	// Create conversion commands based on OSR
	cmdMeasurePres = MS5607_CONV_PRES | config->osr;
	cmdMeasureTemp = MS5607_CONV_TEMP | config->osr;

	// Set measurement delay (ms) based on OSR
	switch (config->osr) 
	{
		case MS5607_OSR_256:
			measurementDelay = 50;
			break;
		case MS5607_OSR_512:
			measurementDelay = 110;
			break;
		case MS5607_OSR_1024:
			measurementDelay = 210;
			break;
		case MS5607_OSR_2048:
			measurementDelay = 410;
			break;
		case MS5607_OSR_4096:
			measurementDelay = 822;
	}

}

void MS5607_init()
{
	MS5607_disable();

	MS5607_enable();
	HAL_SPI_Transmit(config->spi, &CMD_MS5607_RESET, 1, 100);
	MS5607_disable();

	HAL_Delay(3);

	MS5607_readProm();

	state = MS5607_STARTUP;

	log_status("INFO", "Initialized MS5607");
}

void MS5607_readProm()
{
	uint8_t cmdReadPromAddress;
	uint16_t* promDataPtr = (uint16_t*)&promData;
	uint8_t responseBuffer[2];

	// Loop over PROM registers C1 through C6
	for (uint8_t promAddr = 1; promAddr <= 6; ++promAddr)
	{
		// Prom Read Sequence: 1 0 1 0 Ad2 Ad1 Ad0 0
		cmdReadPromAddress = MS5607_READ_PROM | (promAddr<<1);
		
		MS5607_enable();
		HAL_SPI_Transmit(config->spi, &cmdReadPromAddress, 1, 100);
		HAL_SPI_Receive(config->spi, (uint8_t*)responseBuffer, 2, 100);
		MS5607_disable();
		

		// Load response into promData struct
		promDataPtr[promAddr-1] = ((uint16_t)responseBuffer[0]<<8) | (uint16_t)responseBuffer[1];
	}
}

MS5607_CompVal MS5607_getCompValues()
{
	MS5607_CompVal compVals;

	uint32_t digTemp = rawValues.temp;
	uint32_t digPres = rawValues.pres;

	// dT = D2 - TREF
	int32_t deltaTemp = digTemp - ((int32_t)promData.tRef << 8);
	
	// TEMP = 20°C + dT*TEMPSENS
	int32_t temp1 = 2000 + deltaTemp*((int32_t)promData.tempSens << 23);

	// OFF = OFFT1 + TCO*dT
	int64_t offset = ((int64_t)promData.presOffset << 17) 
				   + (((int64_t)promData.tco*deltaTemp) >> 6);
	
	// SENS = SENST1 + TCS* dT
	int64_t sensitivity = ((int64_t)promData.presSens << 16)
						+ (((int64_t)promData.tcs*deltaTemp) >> 7);

	// Second order compensation
	if (temp1 < 2000)
	{
		int32_t temp2 = (deltaTemp*deltaTemp) >> 31;
		int32_t temp2000 = temp1 - 2000;
		int64_t offset2 = 61*((int64_t)temp2000 * (int64_t)temp2000) >> 4;
		int64_t sense2 = ((int64_t)temp2000 * (int64_t)temp2000) << 1;

		if (temp1 < -1500)
		{
			int32_t temp1500 = temp1 + 1500;
			offset2 += 15 * ((int64_t)temp1500 * (int64_t)temp1500);
			sense2 += ((int64_t)temp1500 * (int64_t)temp1500) << 3;
		}

		temp1 -= temp2;
		offset -= offset2;
		sensitivity -= sense2;
	}

	// Populate and return the compensated values
	compVals.pres = (((int64_t)(digPres*sensitivity) >> 21) - offset) >> 15;
	compVals.temp = temp1;

	return compVals;
}

void MS5607_readUncompPres()
{
	state = MS5607_PRES_READ;

	// Send convert command over SPI1
	MS5607_enable();
	HAL_SPI_Transmit(config->spi, &cmdMeasurePres, 1, 100);
	MS5607_disable();

	// Wait for conversion (based on OSR)
	HAL_TIM_Base_Start_IT(config->timer);
}

void MS5607_readUncompTemp()
{
	state = MS5607_TEMP_READ;

	// Send convert command over SPI1
	MS5607_enable();
	HAL_SPI_Transmit(config->spi, &cmdMeasureTemp, 1, 100);
	MS5607_disable();
	
	// Wait for conversion (based on OSR)
	HAL_TIM_Base_Start_IT(config->timer);
}

uint32_t MS5607_readADC()
{
	// 3-byte SPI response buffer for 24-bit ADC read
	uint8_t responseBuffer[] = {0,0,0};
	
	MS5607_enable();
	HAL_SPI_Transmit(config->spi, &CMD_MS5607_READ_ADC, 1, 100);
	HAL_SPI_Receive(config->spi, (uint8_t *)responseBuffer, 3, 100);
	MS5607_disable();

	// Convert the 3-byte MSB-first response into a uint32_t
	return ((uint32_t)responseBuffer[0]<<16) | ((uint32_t)responseBuffer[1]<<8) | (uint32_t)responseBuffer[2];
}

void MS5607_enable()
{
	// Set the CSB pin of the MS5607 low to enable
	HAL_GPIO_WritePin(config->csPort, config->csPin, GPIO_PIN_RESET);
}

void MS5607_disable()
{
	// Set the CSB pin of the MS5607 high to disable
	HAL_GPIO_WritePin(config->csPort, config->csPin, GPIO_PIN_SET);
}

void MS5607_TimerCallback()
{
	uint32_t adcReading = MS5607_readADC();
	if (state == MS5607_PRES_READ)
	{
		rawValues.pres = adcReading;
		MS5607_readUncompTemp();
	}
	else if (state == MS5607_TEMP_READ)
	{
		rawValues.temp = adcReading;
		state = MS5607_IDLE;
	}
}

MS5607_RawVal MS5607_getRawValues()
{
	return rawValues;
}

uint16_t MS5607_getMeasurementDelay()
{
	return measurementDelay;
}

MS5607_MeasureState MS5607_getState()
{
	return state;
}
