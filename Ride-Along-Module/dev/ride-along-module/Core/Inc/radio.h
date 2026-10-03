/*
 * radio.h
 *
 *  Created on: Apr 26, 2026
 *      Author: Tommy
 */

#include "bool.h"
#include "logging.h"
#include "stm32u5xx_hal.h"

#ifndef INC_RADIO_H
#define INC_RADIO_H

#define RADIO_ALIVENESS_CHECK		0x35
#define READ_GPS					0x01


// struct init
typedef struct {
	SPI_HandleTypeDef* spi;
	GPIO_TypeDef* csPort;
	uint16_t csPin;
} radio_HandleTypeDef;



void radio_config(radio_HandleTypeDef*);

void radio_init();

void radio_enable();

void radio_disable();


#endif /* INC_RADIO_H */


//
//---------------- RADIO HEADER -------------------
//
//// Prevent recursive inclusion
//#ifndef radio_H
//#define radio_H
//
//#include "stm32u5xx_hal.h"
//#include "bool.h"
//#include "logging.h"
//
//#define hex values for commands/reg values
//
//define structs: typedef struct { } radio_HandleTypeDef;
//
//define states: typedef enum { } radio_MeasureState;
//
//void function declaration
