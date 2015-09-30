#include  <stdio.h>
#include  "netwritter.h"
#include  "simplefont.h"
#include  "nnet.h"

int parse(const char* filename, NNet* pNNetOut);

bool saveImage(const BmpWritter & writter, const char* fname)
{
    FILE* fd = fopen(fname, "w");
    fwrite(writter.binary(), 1, writter.size(), fd);
    fclose(fd);
    return true;
}

int main()
{
    NetWritter writter;
    writter.create(800, 600, BmpWritter::WHITE);
    writter.drawRect(20, 20, 80, 80, BmpWritter::BLACK);

    writter.drawString(20, 100, "Hello, world! 123", BmpWritter::BLACK);


    writter.drawLayer(20, 140, 80, 80, 200, 200, 3, BmpWritter::BLACK);

    saveImage(writter, "out.bmp");

    printf("INFO: start parser test\n");
    NNet nnet;
    parse("/mnt/data/code/DeconvNet/net/flow_fcn/train_test.prototxt", &nnet);
    return 0;
}
