#ifndef __SIMPLE_FONT_H__
#define __SIMPLE_FONT_H__

class SimpleFont
{
public:
    enum {
        _WIDTH = 8,
        _HEIGHT = 13,
    };

    static const unsigned char* getChar(char ch);
private:
};

#endif
