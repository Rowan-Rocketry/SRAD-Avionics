// Prevent recursive inclusion
#ifndef LSM6DSL_H
#define LSM6DSL_H

#include "bool.h"

// LSM6DSL Output Register Addresses
#define LSM6DSL_GYRO_X_L		0x22
#define LSM6DSL_GYRO_X_H		0x23
#define LSM6DSL_GYRO_Y_L		0x24
#define LSM6DSL_GYRO_Y_H		0x25
#define LSM6DSL_GYRO_Z_L		0x26
#define LSM6DSL_GYRO_Z_H		0x27

#define LSM6DSL_ACCEL_X_L		0x28
#define LSM6DSL_ACCEL_X_H		0x29
#define LSM6DSL_ACCEL_Y_L		0x2a
#define LSM6DSL_ACCEL_Y_H		0x2b
#define LSM6DSL_ACCEL_Z_L		0x2c
#define LSM6DSL_ACCEL_Z_H		0x2d

// ODR Configuration
#define LSM6DSL_POWER_DOWN		0x00
#define LSM6DSL_ODR_12_5_HZ		0x10
#define LSM6DSL_ODR_26_HZ		0x20
#define LSM6DSL_ODR_52_HZ		0x30
#define LSM6DSL_ODR_104_HZ		0x40
#define LSM6DSL_ODR_208_HZ		0x50
#define LSM6DSL_ODR_416_HZ		0x60
#define LSM6DSL_ODR_833_HZ		0x70
#define LSM6DSL_ODR_1660_HZ		0x80
#define LSM6DSL_ODR_3330_HZ		0x90
#define LSM6DSL_ODR_6660_HZ		0xa0

// Accelerometer full-scale selection (g)
#define LSM6DSL_ACCEL_FS_PM_2	0x00
#define LSM6DSL_ACCEL_FS_PM_4	0x08
#define LSM6DSL_ACCEL_FS_PM_8	0x0a
#define LSM6DSL_ACCEL_FS_PM_16	0x04

// Gyro full-scale selection (dps)
#define LSM6DSL_GYRO_FS_PM_125	0x02
#define LSM6DSL_GYRO_FS_PM_250	0x00
#define LSM6DSL_GYRO_FS_PM_500	0x04
#define LSM6DSL_GYRO_FS_PM_1000	0x08
#define LSM6DSL_GYRO_FS_PM_2000	0x0C

#define LSM6DSL_READ			0x80
#define LSM6DSL_WRITE			0x7F

// Configuration registers
#define LSM6DSL_INT1_CTRL		0x0D
#define LSM6DSL_INT2_CTRL		0x0E
#define LSM6DSL_CTRL1_XL		0x10
#define LSM6DSL_CTRL2_G			0x11
#define LSM6DSL_CTRL3_C			0x12
#define LSM6DSL_CTRL4_C			0x13
#define LSM6DSL_CTRL5_C			0x14
#define LSM6DSL_CTRL6_C			0x15
#define LSM6DSL_CTRL7_G			0x16
#define LSM6DSL_CTRL8_XL		0x17
#define LSM6DSL_CTRL9_XL		0x18
#define LSM6DSL_CTRL10_C		0x19

// Interrupt control
#define LSM6DSL_ACCEL_READY		0x01
#define LSM6DSL_GYRO_READY		0x02

#define LSM6DSL_SW_RESET		0x01

// Initialization struct for LSM6DSL
typedef struct {
	SPI_HandleTypeDef* spi;
	GPIO_TypeDef* csPort;
	uint16_t csPin;
	uint8_t outputDataRate;
	uint8_t accelFullScale;
	uint8_t gyroFullScale;
} LSM6DSL_HandleTypeDef;

// Configure LSM6DSL
void LSM6DSL_config(LSM6DSL_HandleTypeDef*);

// Pulse chip select and reset
void LSM6DSL_init();

// Update static acceleration vector
void LSM6DSL_updateAccel();

// Update static gyro vector
void LSM6DSL_updateGyro();

// Get current value of static accel vector
void LSM6DSL_getAccel(int16_t* dest);

// Get current value of static gyro vector
void LSM6DSL_getGyro(int16_t* dest);

// Write a value to a specific register
void LSM6DSL_writeRegister(uint8_t reg, uint8_t val);

// Read a 16-bit measurement value from LSM6DSL
int16_t LSM6DSL_readMeasurement(uint8_t addrLow);

// Use chip select pin to enable LSM6DSL
void LSM6DSL_enable();

// Use chip select pin to disable LSM6DSL
void LSM6DSL_disable();

#endif
