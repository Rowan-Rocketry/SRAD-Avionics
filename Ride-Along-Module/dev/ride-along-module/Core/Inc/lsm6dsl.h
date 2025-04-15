// Prevent recursive inclusion
#ifndef LSM6DSL_H
#define LSM6DSL_H

#include "bool.h"

// LSM6DSL Output Register Addresses
#define LSM6DSL_GYRO_X_L	0x22
#define LSM6DSL_GYRO_X_H	0x23
#define LSM6DSL_GYRO_Y_L	0x24
#define LSM6DSL_GYRO_Y_H	0x25
#define LSM6DSL_GYRO_Z_L	0x26
#define LSM6DSL_GYRO_Z_H	0x27

#define LSM6DSL_ACCEL_X_L	0x28
#define LSM6DSL_ACCEL_X_H	0x29
#define LSM6DSL_ACCEL_Y_L	0x2a
#define LSM6DSL_ACCEL_Y_H	0x2b
#define LSM6DSL_ACCEL_Z_L	0x2c
#define LSM6DSL_ACCEL_Z_H	0x2d

// ODR Configuration
#define LSM6DSL_POWER_DOWN	0x00
#define LSM6DSL_ODR_12_5_HZ	0x10
#define LSM6DSL_ODR_26_HZ	0x20
#define LSM6DSL_ODR_52_HZ	0x30
#define LSM6DSL_ODR_104_HZ	0x40
#define LSM6DSL_ODR_208_HZ	0x50
#define LSM6DSL_ODR_416_HZ	0x60
#define LSM6DSL_ODR_833_HZ	0x70
#define LSM6DSL_ODR_1660_HZ	0x80
#define LSM6DSL_ODR_3330_HZ	0x90
#define LSM6DSL_ODR_6660_HZ	0xa0

// Accelerometer full-scale selection
#define LSM6DSL_FS_PM_2G	0x00
#define LSM6DSL_FS_PM_4G	0x08
#define LSM6DSL_FS_PM_8G	0x0a
#define LSM6DSL_FS_PM_16G	0x04

#define LSM6DSL_READ		0x80
#define LSM6DSL_WRITE		0x00

// Initialization struct for LSM6DSL
typedef struct {
	SPI_HandleTypeDef* spi;
	GPIO_TypeDef* csPort;
	uint16_t csPin;
	uint8_t fullScale;
	// ODR
} MS5607_HandleTypeDef;

// Configure LSM6DSL
void LSM6DSL_init(LSM6DSL_HandleTypeDef);

// Get acceleration vector
void LSM6DSL_readAccel(int32_t* ax, int32_t* ay, int32_t* az);

// Get gyro vector
void LSM6DSL_readGyro(int32_t* gx, int32_t* gy, int32_t* gz);

// Check if accelerometer has data
bool LSM6DSL_accelReady();

// Check if the gyro has data
bool LSM6DSL_gyroReady();

// Use chip select pin to enable LSM6DSL
void LSM6DSL_enable();

// Use chip select pin to disable LSM6DSL
void LSM6DSL_disable();
#endif
