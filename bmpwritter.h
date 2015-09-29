#ifndef __BMP_WRITTER_H__
#define __BMP_WRITTER_H__
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define _BMP_COLORS(_FUNC)                                     \
    _FUNC(WHITE, 255, 255, 255),                               \
    _FUNC(BLACK, 0, 0, 0),                                     \
    _FUNC(RED,   0, 0, 255),                                   \
    _FUNC(GREEN, 0, 255, 0),                                   \
    _FUNC(BLUE,  255, 0, 0)                                

#define _BMP_COLOR_DEF(_COLOR, _R, _G, _B)                  _COLOR
    
class BmpWritter 
{
public:
    enum Color{
        _BMP_COLORS(_BMP_COLOR_DEF),
        _PALETTE_COLOR_NUM
    };

public:
    BmpWritter(unsigned int pointSize = 1);
    ~BmpWritter();

    bool    create(unsigned int width, unsigned int height, Color bg);
    bool    fill(Color bg);
    bool    fill(unsigned int fromX, unsigned int fromY, unsigned int toX, unsigned int toY, Color bg);
    bool    drawPoint(unsigned int x, unsigned int y, Color fg);
    bool    drawRect(unsigned int fromX, unsigned int fromY, unsigned int toX, unsigned int toY, Color fg);
    bool    append(unsigned int heightDelta, Color bg);
    bool    drawBitmap(unsigned int width, unsigned int height, unsigned int x, unsigned int y, const unsigned char* data,Color fg);
    bool    drawString(unsigned int x, unsigned int y, const char* text, Color fg);
    bool    drawLine(unsigned int fromX, unsigned int fromY, unsigned int toX, unsigned int toY, Color bg);

    size_t          size() const;
    const char*     binary() const;
    void            clear();   
    unsigned int    pointSize() const;
    bool            empty() const;
    unsigned int    width() const;
    unsigned int    height() const;

protected:
    size_t          getSizeWithoutPadding(unsigned int widthInPixel, unsigned int heightInPixel) const;
    unsigned int    getLinePadding(unsigned int widthInPixel) const;
    bool            fillRect(size_t yminInPixel, size_t ymaxInPixel, size_t xminInPixel, size_t xmaxInPixel, Color clr);
    bool            updateSizeInHeader(unsigned int bmpSiz, unsigned int widthInPixel, unsigned int heighInPixel);
    unsigned int    pointCordToPixelCord(unsigned int cord) const;
    unsigned int    pixelSize() const;
    unsigned int    headerSize() const;

private:
    char*           m_binary;
    unsigned int    m_width;
    unsigned int    m_height;
    unsigned int    m_ptSize;
};

#endif
