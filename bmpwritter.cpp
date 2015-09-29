#include <algorithm>
#include "bmpwritter.h"
#include "simplefont.h"

// raw data offset 54 = 14 + 40
const static unsigned char s_bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};

// header size: 40
// plants: 1 (always 1)
// bpp: 24 (r8, g8, b8)
// comperhension: 0
// sizeOfBmp(for compress): 0
// HRes: 0
// VRes: 0
// ColorCountInPalette: 5
// ColorImportant: 0
const static unsigned char s_bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 8,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, BmpWritter::_PALETTE_COLOR_NUM,0,0,0, 0,0,0,0};


#define _BMP_PALETTE_DEF(_COLOR, _R, _G, _B)      {_R, _G, _B, 0}
const static struct {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char unused;
} s_palette[] = {
    _BMP_COLORS(_BMP_PALETTE_DEF),
};

BmpWritter::BmpWritter(unsigned int pointSize/* = 1*/)
    :m_binary(NULL),m_width(0),m_height(0),m_ptSize(pointSize)
{    
}

BmpWritter::~BmpWritter()
{
    clear();
}

bool BmpWritter::create(unsigned int width, unsigned int height, Color bg)
{
    clear();

    unsigned int widthInPixel = pointCordToPixelCord(width);
    unsigned int heightInPixel = pointCordToPixelCord(height);
    size_t bmpSiz = getSizeWithoutPadding(widthInPixel, heightInPixel);
    unsigned int padding = getLinePadding(widthInPixel)*heightInPixel;

    // malloc 
    m_binary = (char*)malloc(bmpSiz+padding);
    if (!m_binary){
        assert(0);
        return false;
    }

    // initialize to black
    memset(m_binary, 0, bmpSiz+padding);
    memcpy(m_binary, &s_bmpfileheader, sizeof(s_bmpfileheader));
    memcpy(m_binary+sizeof(s_bmpfileheader), &s_bmpinfoheader, sizeof(s_bmpinfoheader));
    memcpy(m_binary+sizeof(s_bmpfileheader)+sizeof(s_bmpinfoheader), s_palette, sizeof(s_palette));

    // update size in header
    updateSizeInHeader((unsigned int)bmpSiz+padding, widthInPixel, heightInPixel);

    m_width = width;
    m_height = height;

    return fill(bg);
}

unsigned int BmpWritter::headerSize() const
{
    return sizeof(s_bmpfileheader)+sizeof(s_bmpinfoheader)+_PALETTE_COLOR_NUM*sizeof(s_palette[0]);
}

bool BmpWritter::updateSizeInHeader(unsigned int bmpSiz, unsigned int widthInPixel, unsigned int heightInPixel)
{
    if (!m_binary){
        assert(0);
        return false;
    }

    *(unsigned int*)(m_binary+2) = bmpSiz;
    *(unsigned int*)(m_binary+sizeof(s_bmpfileheader)+4) = widthInPixel;

    // use nagative height to convert bmp from bottom-up to top-down
    *(int*)(m_binary+sizeof(s_bmpfileheader)+8) = -(int)heightInPixel;
    return true;
}

bool BmpWritter::empty() const
{
    return m_binary?false:true;
}

unsigned int BmpWritter::width() const
{
    return m_width;
}

unsigned int BmpWritter::height() const
{
    return m_height;
}

size_t BmpWritter::size() const
{
    if (empty()){
        return 0;
    }
    unsigned int widthInPixel = pointCordToPixelCord(m_width);
    unsigned int heightInPixel = pointCordToPixelCord(m_height);
    return getSizeWithoutPadding(widthInPixel, heightInPixel) + getLinePadding(widthInPixel)*heightInPixel;
}

unsigned int BmpWritter::pointCordToPixelCord(unsigned int cord) const
{
    return cord*m_ptSize;
}

size_t BmpWritter::getSizeWithoutPadding(unsigned int widthInPixel, unsigned int heightInPixel) const
{
    return headerSize()+pixelSize()*widthInPixel*heightInPixel;
}

const char* BmpWritter::binary() const
{
    return m_binary;
}

unsigned int BmpWritter::getLinePadding(unsigned int widthInPixel) const
{
    return (unsigned int)((4-widthInPixel*pixelSize()%4)%4);
}

bool BmpWritter::fill(Color bg)
{
    if (!m_binary){
        assert(0);
        return false;
    }

    size_t xmin = 0;
    size_t xmax = pointCordToPixelCord(m_width)-1;
    size_t ymin = 0;
    size_t ymax = pointCordToPixelCord(m_height)-1;
    return fillRect(ymin, ymax, xmin, xmax, bg);   
}

bool BmpWritter::drawPoint(unsigned int x, unsigned int y, Color fg)
{
    //printf ("point: (%u, %u)\n", x, y);

    if (!m_binary){
        assert(0);
        return false;
    }
    if (x>=m_width || y>=m_height){
        printf("Error: point(%u,%u) exceed the bitmap size(%u,%u)!\n", x, y, m_width, m_height);
        return false;
    }

    size_t xminInPixel = pointCordToPixelCord(x);
    size_t xmaxInPixel= xminInPixel;
    size_t yminInPixel = pointCordToPixelCord(y);
    size_t ymaxInPixel= yminInPixel;

    // point is consider as 1*1 rect
    return fillRect(yminInPixel, ymaxInPixel, xminInPixel, xmaxInPixel, fg);    
}

bool BmpWritter::fillRect(size_t yminInPixel, size_t ymaxInPixel, size_t xminInPixel, size_t xmaxInPixel, Color clr)
{
    //file the rect line by line
    size_t widthInPixel = pointCordToPixelCord(m_width);
    size_t lineLen = pixelSize()*widthInPixel+getLinePadding((unsigned int)widthInPixel);
    char* imgData = m_binary+headerSize();
    for (size_t y=yminInPixel; y<=ymaxInPixel; ++y){
        size_t offset = y*lineLen+xminInPixel*pixelSize();
        memset(imgData+offset, clr, xmaxInPixel-xminInPixel+1);
    }
    return true;
}

unsigned int BmpWritter::pixelSize() const
{
    return 1;
}

bool BmpWritter::fill(unsigned int fromX, unsigned int fromY, unsigned int toX, unsigned int toY, Color fg)
{
    if (!m_binary){
        assert(0);
        return false;
    }

    size_t xminInPixel = pointCordToPixelCord(std::min(fromX, toX));
    size_t xmaxInPixel = pointCordToPixelCord(std::max(fromX, toX));
    size_t yminInPixel = pointCordToPixelCord(std::min(fromY, toY));
    size_t ymaxInPixel = pointCordToPixelCord(std::max(fromY, toY));

    return fillRect(yminInPixel, ymaxInPixel, xminInPixel, xmaxInPixel, fg);
}

bool BmpWritter::drawRect(unsigned int fromX, unsigned int fromY, unsigned int toX, unsigned int toY, Color fg)
{
    if (!fill(fromX, fromY, toX, fromY, fg)){
        assert(0);
        return false;
    }
    if (!fill(fromX, fromY, fromX, toY, fg)){
        assert(0);
        return false;
    }
    if (!fill(toX, toY, fromX, toY, fg)){
        assert(0);
        return false;
    }
    if (!fill(toX, toY, toX, fromY, fg)){
        assert(0);
        return false;
    }
    return true;
}

bool BmpWritter::drawBitmap(unsigned int width, unsigned int height, unsigned int x, unsigned int y, const unsigned char* data,Color fg)
{
    assert(width%8 == 0);
    if (!data){
        return false;
    } 

    int index = 0;
    for (unsigned int h=0; h<height; ++h){
        for (unsigned int w=0; w<width; w+=8){
            unsigned char ele = data[index++];
            for (int bit=0; bit<8; ++bit){
                if (ele & 0x1<<(7-bit)){
                    drawPoint(x+w*8+bit, y+height-h, fg);
                }
            }
        }
    }
    return true;
}

bool BmpWritter::drawString(unsigned int x, unsigned int y, const char* text, Color fg)
{
    printf ("string: (%u, %u, \"%s\")\n", x, y, text);

    unsigned startX = x;
    for (int i=0; i<(int)strlen(text); ++i){
        drawBitmap(SimpleFont::_WIDTH, SimpleFont::_HEIGHT, startX, y, SimpleFont::getChar(text[i]), fg);
        startX += (SimpleFont::_WIDTH + SimpleFont::_WIDTH/2);
    }
    return true;
}

void BmpWritter::clear()
{
    if (m_binary){
        free(m_binary);
        m_binary = NULL;
        m_width = 0;
        m_height = 0;
    }
}

unsigned int BmpWritter::pointSize() const
{
    return m_ptSize;
}

bool BmpWritter::append(unsigned int heightDelta, Color bg)
{
    unsigned int widthInPixel = pointCordToPixelCord(m_width);
    unsigned int newHeight = m_height+heightDelta;
    unsigned int newHeightInPixel = pointCordToPixelCord(newHeight);
    size_t bmpSiz = getSizeWithoutPadding(widthInPixel, newHeightInPixel);
    unsigned int padding = getLinePadding(widthInPixel)*newHeightInPixel;

    // realloc memory
    char* oldBinary = m_binary;
    m_binary = (char*)realloc(m_binary, bmpSiz+padding);
    if (!m_binary){
        m_binary = oldBinary;
        return false;
    }

    // update size in header
    updateSizeInHeader((unsigned int)bmpSiz, widthInPixel, newHeightInPixel);    

    unsigned int fromX = 0;
    unsigned int fromY = m_height;
    unsigned int toX = m_width-1;
    unsigned int toY = newHeight-1;

    m_height = newHeight;

    // fill the new append binary with color
    fill(fromX, fromY, toX, toY, bg);
    return true;
}

bool BmpWritter::drawLine(unsigned int fromX, unsigned int fromY, unsigned int toX, unsigned int toY, Color bg)
{
    printf ("line: (%u, %u) -> (%u, %u)\n", fromX, fromY, toX, toY);

    if (fromX == toX || fromY == toY){
        return fill(fromX, fromY, toX, toY, bg);
    }

    if (std::abs((int)fromX-(int)toX) > std::abs((int)fromY-(int)toY)){
        float gradient = ((float)((int)toY-(int)fromY))/((int)toX-(int)fromX);
        if (fromX < toX){
            for (unsigned int i=fromX; i<=toX; ++i){
                drawPoint(i, fromY+gradient*(i-fromX), bg);
            }
        } else {
            for (unsigned int i=fromX; i>=toX; --i){
                drawPoint(i, fromY-gradient*(fromX-i), bg);
            }
        }
    } else {
        float gradient = ((float)((int)toX-(int)fromX))/((int)toY-(int)fromY);
        if (fromY < toY){
            for (unsigned int i=fromY; i<=toY; ++i){
                drawPoint(fromX+gradient*(i-fromY), i, bg);
            }
        } else {
            for (unsigned int i=fromY; i>=toY; --i){
                drawPoint(fromX-gradient*(fromY-i), i, bg);
            }
        }
    }

    return true;
}
