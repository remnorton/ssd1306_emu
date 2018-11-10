/*
 * ssd1306emu.cpp
 *
 *  Created on: 29.10.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */

#include <QDebug>
#include <QtCore>
#include <QByteArray>
#include <QOpenGLWidget>
#include "ssd1306emu.h"
#include "emuscreen.h"
#include "transport.h"

//
//
//
//! addresing mode
enum
{
    SSDAM_Horizontal = 0,
    SSDAM_Vertical,
    SSDAM_Page = 16
};


//
//
//
static SSD1306EMU_Init_t* instance = 0;
static EmuScreen* screen = 0;
static QByteArray gram;
static uint8_t page = 0;
static uint8_t start_line = 0;
static uint8_t start_col = 0;
static uint8_t stop_col = 0;
static uint8_t addr_mode = 0;
static uint8_t line_offset = 0;
//
//private forwards
//

void i2cReceiver(uint8_t* data, uint16_t size, uint8_t mem_addr, uint8_t meme_inc);

//
//
//
uint8_t ssd1306emu_init(SSD1306EMU_Init_t* init)
{
    if (!init) return 0;
    if (instance) return 0;
    instance = init;
    if ((init->resolution == SSD1306_RES_128x32) && (init->color == SSD1306_CM_BlueYellow))
        init->color = SSD1306_CM_Blue;
    int w = 128;
    int h = (instance->resolution == SSD1306_RES_128x32)?32:64;
    gram.resize(w*h/8);
    memset(gram.data(), 0xff , gram.count());
    screen = new EmuScreen(0, Qt::WindowStaysOnTopHint);
    screen->setGRAM((uint8_t*)gram.data());
    screen->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    w = w * (instance->pix_spacing+instance->scale)+instance->pix_spacing;
    h = h * (instance->pix_spacing+instance->scale)+instance->pix_spacing;
    screen->setMinimumSize(w,h);
    screen->setMaximumSize(w,h);
    screen->updateGeometry();
    screen->setWindowTitle("SSD1306 emulator");
    screen->setLedMatrixData(instance);
    screen->setCloseCallback(instance->onCloseCallback);
    screen->show();
    addTransportReceiver(VTT_I2C, 512, instance->i2c_addr, i2cReceiver);
    return 1;
}

void ssd1306emu_stop()
{
    if (!instance) return;
    instance = 0;
    if (screen)
    {
        screen->close();
        screen->deleteLater();
        screen = 0;
    }
}

//
//private members
//

void i2cReceiver(uint8_t* data, uint16_t size, uint8_t mem_addr, uint8_t meme_inc)
{
    Q_UNUSED(meme_inc);
    if (mem_addr == 0x00) //commands
    {
        while (size)
        {
            uint8_t cmd_code = *data;
            data++;
            size--;

            if ((cmd_code >= 0x40) && (cmd_code <= 0x7f))
            {
                start_line = cmd_code - 0x40;
                screen->setLineOffset(line_offset+start_line);
            }
            if (cmd_code <= 0x0f)
                start_col = cmd_code;

            if ((cmd_code > 0x0f) && (cmd_code <= 0x1f))
                stop_col = cmd_code;
            
            switch(cmd_code)
            {
                case 0xE3: //NOP
                {
                    break;
                }
                case 0x21: //Set Column Address
                {
                    data += 2;
                    size -= 2;
                    break;
                }
                case 0x22: //Set Page Address
                {
                    data += 2;
                    size -= 2;
                    break;
                }
                case 0xAE: //display off
                {
                    screen->setEnabled(false);
                    break;
                }
                case 0xAF: //display on
                {
                    screen->setEnabled(true);
                    break;
                }
                case 0x20:  //Set Memory Addressing Mode
                {
                    addr_mode = *data;
                    data++;
                    size--;
                    break;
                }
                case 0xB0: //Set Page Start Address for Page Addressing Mode,0-7
                case 0xB1:
                case 0xB2:
                case 0xB3:
                case 0xB4:
                case 0xB5:
                case 0xB6:
                case 0xB7:
                {
                    page = cmd_code & 0x07;
                    break;
                }
                case 0xC0: //Set COM Output Scan Direction REVERSE
                case 0xC8: //Set COM Output Scan Direction NORMAL
                {
                    screen->setFlipY(cmd_code == 0xC0);
                    break;
                }
                case 0xA0:  //Set segment re-map 127 to 0
                case 0xA1:  //Set segment re-map 0 to 127
                {
                    screen->setFlipX(cmd_code == 0xA0);
                    break;
                }
                case 0xA6:
                {
                    screen->setInverted(false);
                    break;
                }
                case 0xA7:
                {
                    screen->setInverted(true);
                    break;
                }
                case 0x81:  //Contrast control
                {
                    uint8_t contrast = *data;
                    screen->setContrast(contrast);
                    data++;
                    size--;
                    //TODO: set contrast
                    break;
                }
                case 0xA8:  //Multiplex ratio
                {
                    uint8_t ratio = *data;
                    data++;
                    size--;
                    break;
                }
                case 0xA4:
                case 0xA5:
                {
                    break;
                }
                case 0xD3: //display offset
                {
                    line_offset = *data;
                    screen->setLineOffset(line_offset+start_line);
                    data++;
                    size--;
                    break;
                }
                case 0xD5:
                {
                    uint8_t ratio = *data;
                    data++;
                    size--;
                    break;
                }
                case 0xD9:
                {
                    uint8_t period = *data;
                    data++;
                    size--;
                    break;
                }
                case 0xDA:
                {
                    uint8_t pinconf = *data;
                    data++;
                    size--;
                    break;
                }
                case 0xDB:
                {
                    uint8_t vcom = *data;
                    data++;
                    size--;
                    break;
                }
                case 0x8D: //DC-DC enable
                {
                    data++;
                    size--;
                    break;
                }

            }
        }
    }
    if (mem_addr >= 0x40) //data
    {
        uint8_t pageLen = 128;
        uint16_t offset = 0;

        switch(addr_mode)
        {
            case SSDAM_Horizontal:
            {
                if (gram.count() >= (size)) memcpy(gram.data(), data, size);
                else
                {
                    memcpy(gram.data(), data, gram.count());
                    memcpy(gram.data(), data +gram.count(), size-gram.count());
                }
                break;
            }
            case SSDAM_Vertical:
            {
                //TODO: Not realised
                break;
            }
            case SSDAM_Page:
            {
                offset = page*pageLen+start_col;
                if (size > pageLen) size = pageLen;
                if (offset < gram.count())
                {
                    if (gram.count() >= (offset+size)) memcpy(gram.data()+offset, data, size);
                    else memcpy(gram.data()+offset, data, gram.count()-offset);
                }
                break;
            }
        }
        screen->repaint();
    }
}
