/*
 * transport.cpp
 *
 *  Created on: 31.10.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */

#include "transport.h"

//
//
//
typedef struct
{
    uint8_t type;
    uint16_t maxSize;
    uint16_t addr;
    transportFunc fnx;
}Transport_Handler_t;

//
//
//
static QList<Transport_Handler_t> connections;

//
//private forwards
//
Transport_Handler_t getTransHandler(uint8_t type, uint16_t addr)
{
    foreach(Transport_Handler_t th, connections)
    {
        if ((th.type == type) && (th.addr == addr))
            return th;
    }
    return (Transport_Handler_t){0,0,0,0};
}

//
//
//
void addTransportReceiver(uint8_t type, uint16_t maxSize, uint16_t addr, transportFunc fnx)
{
    Transport_Handler_t th = {type, maxSize, addr, fnx};
    connections.append(th);
}

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef* handle, uint8_t dev_addr, uint8_t mem_addr, uint8_t meme_inc, uint8_t* data, uint16_t length, uint16_t timeout)
{
    Q_UNUSED(handle);
    Q_UNUSED(timeout);
    Transport_Handler_t th = getTransHandler(VTT_I2C, dev_addr);
    if (th.fnx)
        th.fnx(data, length, mem_addr, meme_inc);
    return 1;
}

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    Q_UNUSED(hspi);
    Q_UNUSED(Timeout);
    Transport_Handler_t th = getTransHandler(VTT_SPI, 0);
    if (th.fnx)
        th.fnx(pData, Size, 0, 0);
    return 1;
}
