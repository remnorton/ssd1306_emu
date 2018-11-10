/*
 * ssd1306emu.h
 *
 *  Created on: 29.10.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef SSD1306EMU_H
#define SSD1306EMU_H
#include <stdint.h>

typedef void (*exitFunc)();

typedef struct
{
    uint8_t color;
    uint8_t resolution;
    uint8_t i2c_addr;
    uint8_t scale;
    uint8_t pix_spacing;
    exitFunc onCloseCallback;
}SSD1306EMU_Init_t;

//!Sizes
enum
{
    SSD1306_RES_128x32 = 0,
    SSD1306_RES_128x64
};

//! Color modes
enum
{
    SSD1306_CM_Blue = 0,
    SSD1306_CM_White,
    SSD1306_CM_BlueYellow
};


uint8_t ssd1306emu_init(SSD1306EMU_Init_t* init);
void ssd1306emu_stop();

#endif // SSD1306EMU_H
