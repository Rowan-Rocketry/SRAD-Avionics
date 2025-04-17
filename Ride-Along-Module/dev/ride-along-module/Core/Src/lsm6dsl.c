#include "lsm6dsl.h"

static LSM6DSL_HandleTypeDef* config;

void LSM6DSL_init(LSM6DSL_HandleTypeDef* LSM6DSL_initStruct)
{
	config = LSM6DSL_initStruct;

	switch (config->accelFullScale)
	{
		case LSM6DSL_ACCEL_FS_PM_2:
			config->accelSensitivity = 0.061f;
			break;
		case LSM6DSL_ACCEL_FS_PM_4:
			config->accelSensitivity = 0.122f;
			break;
		case LSM6DSL_ACCEL_FS_PM_8:
			config->accelSensitivity = 0.244f;
			break;
		case LSM6DSL_ACCEL_FS_PM_16:
			config->accelSensitivity = 0.488f;
	}

	switch (config->gyroFullScale)
	{
		case LSM6DSL_GYRO_FS_PM_125:
			config->gyroSensitivity = 4.375f;
			break;
		case LSM6DSL_GYRO_FS_PM_250:	
			config->gyroSensitivity = 8.75f;
			break;
		case LSM6DSL_GYRO_FS_PM_500:	
			config->gyroSensitivity = 17.50f;
			break;
		case LSM6DSL_GYRO_FS_PM_1000:	
			config->gyroSensitivity = 35.0f;
			break;
		case LSM6DSL_GYRO_FS_PM_2000:	
			config->gyroSensitivity = 70.0f;
	}
}

void LSM6DSL_readAccel(int16_t* ax, int16_t* ay, int16_t* az)
{
	uint16_t axRaw = LSM6DSL_readMeasurement(LSM6DSL_ACCEL_X_L);
	uint16_t ayRaw = LSM6DSL_readMeasurement(LSM6DSL_ACCEL_Y_L);
	uint16_t azRaw = LSM6DSL_readMeasurement(LSM6DSL_ACCEL_Z_L);

	*ax = (uint16_t)(axRaw * config->accelSensitivity);
	*ay = (uint16_t)(ayRaw * config->accelSensitivity);
	*az = (uint16_t)(azRaw * config->accelSensitivity);
}

void LSM6DSL_readGyro(int16_t* gx, int16_t* gy, int16_t* gz)
{	
	uint16_t gxRaw = LSM6DSL_readMeasurement(LSM6DSL_GYRO_X_L);
	uint16_t gyRaw = LSM6DSL_readMeasurement(LSM6DSL_GYRO_Y_L);
	uint16_t gzRaw = LSM6DSL_readMeasurement(LSM6DSL_GYRO_Z_L);

	*gx = (uint16_t)(gxRaw * config->gyroSensitivity);
	*gy = (uint16_t)(gyRaw * config->gyroSensitivity);
	*gz = (uint16_t)(gzRaw * config->gyroSensitivity);
}

int16_t LSM6DSL_readMeasurement(uint8_t addrLow)
{
	uint8_t readCommand = LSM6DSL_READ | addrLow;

	// Store the buffer bytes (8-bit register values)
	// and 16-bit signed integer result in same memory.
	union buffer { uint8_t bytes; int16_t result; };
	union buffer buffer;

	LSM6DSL_enable();
	HAL_SPI_Transmit(config->spi, &readCommand, 1, 100);
	HAL_SPI_Receive(config->spi, &buffer.bytes, 2, 100);
	LSM6DSL_disable();

	return buffer.result;
}

void LSM6DSL_enable()
{
	// Set cs low to enable
	HAL_GPIO_WritePin(config->csPort, config->csPin, GPIO_PIN_RESET);
}

void LSM6DSL_disable()
{
	// Set cs high to disable
	HAL_GPIO_WritePin(config->csPort, config->csPin, GPIO_PIN_SET);
}
