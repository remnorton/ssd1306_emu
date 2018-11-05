/*
 * trandport.h
 *
 *  Created on: 31.10.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QtCore>
#include <stdint.h>


enum
{
    VTT_I2C,
    VTT_SPI,
};

typedef void (*transportFunc)(uint8_t* /*data*/, uint16_t /*size*/, uint8_t /*mem_addr*/, uint8_t /*meme_inc*/);

typedef uint8_t HAL_StatusTypeDef;
typedef uint8_t I2C_HandleTypeDef;
typedef uint8_t SPI_HandleTypeDef;


void addTransportReceiver(uint8_t type, uint16_t maxSize, uint16_t addr, transportFunc fnx);

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef* handle, uint8_t dev_addr, uint8_t mem_addr, uint8_t meme_inc, uint8_t* data, uint16_t length, uint16_t timeout);
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout);

#endif // TRANSPORT_H
