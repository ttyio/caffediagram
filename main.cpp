#include  <stdio.h>
#include  "netwritter.h"
#include  "simplefont.h"
#include  "nnet.h"

int parse(const char* filename, NNet* pNNetOut);

int main()
{
    NNet nnet;
    parse("/mnt/data/code/DeconvNet/net/flow_fcn/train_test.prototxt", &nnet);
    nnet.draw("out.bmp");
    return 0;
}
