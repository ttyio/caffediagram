#ifndef __NET_WRITTER_H__
#define __NET_WRITTER_H__

#include  "bmpwritter.h"

class NetWritter
    :public BmpWritter 
{
    enum {
        _LAYER_N = 2,
    };

public:
    bool drawLayer(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned w, unsigned h, unsigned c, Color fg);

    bool drawCube(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned w, unsigned h, unsigned c);

    bool drawArrow(unsigned int x, unsigned int y);

private:
};

#endif
