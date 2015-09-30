#include  <stdio.h>
#include  "nnet.h"

NNet::NNet()
{
}

NNet::~NNet()
{
}

bool NNet::beginLayer()
{
    printf("Begin layer");
}

bool NNet::endLayer()
{
    printf("End layer");
}

bool NNet::setAttri(int level, const char* curAttriBlock, const char* attri, const char* value)
{
    printf("setAttri(%d, %s, %s, %s)", level, curAttriBlock, attri, value);
}
