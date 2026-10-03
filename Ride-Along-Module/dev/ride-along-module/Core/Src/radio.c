/*
 * radio.c
 *
 *  Created on: Apr 26, 2026
 *      Author: Tommy
 */

#include "radio.h"

static radio_HandleTypeDef* config;

void radio_config(radio_HandleTypeDef* radio_initStruct){

	config = radio_initStruct;
}


void radio_init(){


	/**
	  * @brief  Transmit and Receive an amount of data in blocking mode.
	  * @param  hspi   : pointer to a SPI_HandleTypeDef structure that contains
	  *                  the configuration information for SPI module.
	  * @param  pTxData: pointer to transmission data buffer
	  * @param  pRxData: pointer to reception data buffer
	  * @param  Size   : amount of data to be sent and received
	  * @param  Timeout: Timeout duration
	  * @retval HAL status
	  */
	uint8_t cmdVal = RADIO_ALIVENESS_CHECK;
	uint8_t responseBuff[1];
	radio_disable();
	radio_enable();
//	HAL_SPI_TransmitReceive(SPI_HandleTypeDef* hspi, uint8_t* pTxData, uint8_t* pRxData, uint16_t Size, uint32_t Timeout);
	HAL_SPI_TransmitReceive(config->spi, &cmdVal, responseBuff, 1, 4);
	radio_disable();

}


void radio_enable()
{
	// Set cs low to enable
	HAL_GPIO_WritePin(config->csPort, config->csPin, GPIO_PIN_RESET);
}

void radio_disable()
{
	// Set cs high to disable
	HAL_GPIO_WritePin(config->csPort, config->csPin, GPIO_PIN_SET);
}



//---------------- RADIO MAIN -------------------
//
//
//radio_init()
//radio_updateTelemetry()
//radio_writeCommand()
//radio_enable()
//radio_disable()
