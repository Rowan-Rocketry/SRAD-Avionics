#include "lsm6dsl.h"

static LSM6DSL_HandleTypeDef* config;

static int16_t accel[3];
static int16_t gyro[3];

void LSM6DSL_config(LSM6DSL_HandleTypeDef* LSM6DSL_initStruct)
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

void LSM6DSL_init()
{
	LSM6DSL_disable();

	// Send software reset
	LSM6DSL_writeRegister(LSM6DSL_CTRL3_C, LSM6DSL_SW_RESET);

	// Configure interrupts when data is ready
	LSM6DSL_writeRegister(LSM6DSL_INT1_CTRL, LSM6DSL_ACCEL_READY);
	LSM6DSL_writeRegister(LSM6DSL_INT2_CTRL, LSM6DSL_GYRO_READY);

	// Configure data rate and measurement scale
	LSM6DSL_writeRegister(LSM6DSL_CTRL1_XL, config->outputDataRate | config->accelFullScale);
	LSM6DSL_writeRegister(LSM6DSL_CTRL2_G, config->outputDataRate | config->gyroFullScale);
}

void LSM6DSL_updateAccel()
{
	int16_t axRaw = LSM6DSL_readMeasurement(LSM6DSL_ACCEL_X_L);
	int16_t ayRaw = LSM6DSL_readMeasurement(LSM6DSL_ACCEL_Y_L);
	int16_t azRaw = LSM6DSL_readMeasurement(LSM6DSL_ACCEL_Z_L);

	accel[0] = (int16_t)(axRaw * config->accelSensitivity);
	accel[1] = (int16_t)(ayRaw * config->accelSensitivity);
	accel[2] = (int16_t)(azRaw * config->accelSensitivity);
}

void LSM6DSL_updateGyro()
{	
	int16_t gxRaw = LSM6DSL_readMeasurement(LSM6DSL_GYRO_X_L);
	int16_t gyRaw = LSM6DSL_readMeasurement(LSM6DSL_GYRO_Y_L);
	int16_t gzRaw = LSM6DSL_readMeasurement(LSM6DSL_GYRO_Z_L);

	gyro[0] = (int16_t)(gxRaw * config->gyroSensitivity);
	gyro[1] = (int16_t)(gyRaw * config->gyroSensitivity);
	gyro[2] = (int16_t)(gzRaw * config->gyroSensitivity);
}

void LSM6DSL_writeRegister(uint8_t reg, uint8_t val)
{
	uint8_t buffer[] = {
		reg & LSM6DSL_WRITE,
		val
	};

	LSM6DSL_enable();
	HAL_SPI_Transmit(config->spi, &buffer, 2, 100);
	LSM6DSL_disable();
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

void LSM6DSL_getAccel(int16_t* dest)
{
	dest[0] = accel[0];
	dest[1] = accel[1];
	dest[2] = accel[2];
}

void LSM6DSL_getGyro(int16_t* dest)
{
	dest[0] = gyro[0];
	dest[1] = gyro[1];
	dest[2] = gyro[2];
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
