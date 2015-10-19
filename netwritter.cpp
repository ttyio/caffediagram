#include    <stdlib.h>
#include    <stdio.h>
#include    "netwritter.h"
#include    "simplefont.h"


bool NetWritter::drawLayer(const char* name, unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned w, unsigned h, unsigned c, Color fg)
{
    int gradientMargin = width/4;
    int OFF = 4;

    drawLine(x+gradientMargin, y, x+gradientMargin+width, y, fg);
    drawLine(x+gradientMargin, y, x, y+height, fg);
    drawLine(x, y+height, x+width, y+height, fg);
    drawLine(x+gradientMargin+width, y, x+width, y+height, fg);

    unsigned int startX = x+OFF;
    unsigned int startY = y+height;
    int layerMargin = height/5;

    for (int i=1; i<_LAYER_N; ++i){
        unsigned int Xoff = (unsigned int)((float)gradientMargin/height*layerMargin);
        drawLine(startX, startY+layerMargin, startX+width, startY+layerMargin, fg);
        drawLine(startX, startY+layerMargin, startX+Xoff, startY, fg);
        drawLine(startX+gradientMargin+width, startY-height+layerMargin, startX+gradientMargin+width-Xoff, startY-height+layerMargin, fg);
        drawLine(startX+gradientMargin+width, startY-height+layerMargin, startX+width, startY+layerMargin, fg);


        startY += layerMargin;
        startX += OFF;
    }

    char buf[10];

    if (w){
        sprintf(buf, "%u", w);
        drawString(startX+width/3, startY-layerMargin, buf, fg);
    }

    if (h){
        sprintf(buf, "%u", h);
        drawString(startX+width, startY-height/2, buf, fg);
    }

    if (c){
        sprintf(buf, "%u", c);
        drawString(startX+gradientMargin+width, startY-height-SimpleFont::_HEIGHT, buf, fg);
    }

    if (name){
        char* newname = strdup(name);
        if (strlen(newname) > 8){
            newname[5] = '.';
            newname[6] = '.';
            newname[7] = '\0';
        }
        drawString(x, y+height+layerMargin, newname, fg);
        free(newname);
    }
    return true;
}
