/*
 * emuscreen.h
 *
 *  Created on: 29.10.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef EMUSCREEN_H
#define EMUSCREEN_H

#include <QtCore>
#include <QWidget>
#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QCloseEvent>
#include "ssd1306emu.h"

//
//
//
#define _X_SPACING  0
#define _Y_SPACING  0

#define _X_SCALE    2
#define _Y_SCALE    2


//
//
//


class EmuScreen : public QOpenGLWidget
{
public:
    explicit EmuScreen(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    void setGRAM(uint8_t* gramPtr);
    void setLedMatrixData(SSD1306EMU_Init_t* ledMatData);
    void setEnabled(bool e);
    void setInverted(bool i);
    void setContrast(uint8_t level);
    void setLineOffset(uint8_t offset);
    void setFlipY(bool flip);
    void setFlipX(bool flip);
    void setCloseCallback(exitFunc cb);

private:
    QRgb getColor(int x, int y);

protected:
    void paintEvent(QPaintEvent* e);
    void closeEvent(QCloseEvent* e);

private:
    uint8_t* GRAM = 0;
    uint8_t  colorMode = SSD1306_CM_Blue;
    SSD1306EMU_Init_t* instance = 0;
    bool enabled = true;
    bool inverted = false;
    uint8_t contrast = 0x80;
    QSize screenSize;
    uint8_t lineOffset = 0;
    bool flipY = false;
    bool flipX = false;
    exitFunc onClose = 0;
};

#endif // EMUSCREEN_H
