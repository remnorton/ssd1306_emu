/*
 * emuscreen.cpp
 *
 *  Created on: 29.10.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */

//#include <QDebug>
#include <QPainter>
#include <QColor>
#include "emuscreen.h"


#define _WHITE_     qRgb(255,255,255)
#define _YELLOW_    qRgb(255,255,0)
#define _BLUE_      qRgb(0,170,255)


EmuScreen::EmuScreen(QWidget* parent, Qt::WindowFlags f)
    :QOpenGLWidget(parent, f)
{
    //qDebug() << Q_FUNC_INFO;
    setWindowFlag(Qt::WindowStaysOnTopHint);
}

void EmuScreen::setGRAM(uint8_t* gramPtr)
{
    //qDebug() << Q_FUNC_INFO;
    GRAM = gramPtr;
}

void EmuScreen::setLedMatrixData(SSD1306EMU_Init_t* ledMatData)
{
    if (instance) return;
    instance = ledMatData;
    screenSize.setWidth(128);
    screenSize.setHeight((instance->resolution == SSD1306_RES_128x32)?32:64);
}

void EmuScreen::setEnabled(bool e)
{
    enabled = e;
    repaint();
}

void EmuScreen::setInverted(bool i)
{
    inverted = i;
    repaint();
}

void EmuScreen::setContrast(uint8_t level)
{
    contrast = level;
    if (contrast < 50) contrast = 50;
    repaint();
}

void EmuScreen::setLineOffset(uint8_t offset)
{
    lineOffset = offset%screenSize.height();
    repaint();
}

void EmuScreen::setFlipY(bool flip)
{
    flipY = flip;
    repaint();
}

void EmuScreen::setFlipX(bool flip)
{
    flipX = flip;
    repaint();
}

void EmuScreen::setCloseCallback(exitFunc cb)
{
    onClose = cb;
}

QRgb EmuScreen::getColor(int x, int y)
{
    if (!GRAM) return 0;
    if (!instance) return 0;
    uint8_t hl = (GRAM[x + (y / 8) * screenSize.width()] & (1 << (y % 8)) );
    QRgb result = qRgb(0,0,0);

    if (inverted) hl = !hl;
    if (!enabled) hl = 0;

    switch(instance->color)
    {
        case SSD1306_CM_Blue:
        {
            if (hl > 0) result = _BLUE_;
            break;
        }
        case SSD1306_CM_BlueYellow:
        {
            if (lineOffset)
            {
                y -= lineOffset;
                if (y < 0) y = screenSize.height()+y;
            }
            if (flipY) y = screenSize.height()-y-1;

            if (hl > 0) result = (y > 15)?_BLUE_:_YELLOW_;
            break;
        }
        case SSD1306_CM_White:
        {
            if (hl > 0) result = _WHITE_;
            break;
        }
    }

    return result;
}

void EmuScreen::paintEvent(QPaintEvent* e)
{
    if (GRAM)
    {
        QPainter painter(this);
        for (int y = 0; y < screenSize.height(); y++)
        {
            for (int x = 0; x < screenSize.width(); x++)
            {
                int cx = flipX?(screenSize.width()-x-1):x;
                int cy = y;

                if (lineOffset)
                {
                    cy = y - lineOffset;
                    if (cy < 0) cy = screenSize.height()+cy;
                }

                if (flipY) cy = screenSize.height()-cy-1;

                int sx = instance->pix_spacing + (instance->pix_spacing + instance->scale)*cx;
                int sy = instance->pix_spacing + (instance->pix_spacing + instance->scale)*cy;

                QRgb rgbcl = getColor(x,y);
                QColor cl = QColor(QColor(rgbcl));
                if (enabled && (rgbcl > qRgb(0,0,0)))
                {
                    int h,s,l;
                    cl.getHsl(&h, &s, &l);
                    if (rgbcl < qRgb(255,255,255)) cl.setHsl(h, s, contrast/2);
                    else cl.setHsl(h, s, contrast);
                }
                painter.fillRect(sx, sy, instance->scale, instance->scale, QBrush(cl));
            }
        }
        painter.end();
    }
    e->accept();
}

void EmuScreen::closeEvent(QCloseEvent* e)
{
    if (onClose) onClose();
    QOpenGLWidget::closeEvent(e);
}
