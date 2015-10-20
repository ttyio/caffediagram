#include  <stdio.h>
#include  "netwritter.h"
#include  "simplefont.h"
#include  "nnet.h"

#define OUTPUT "out.bmp"

extern int parse(const char* filename, NNet* pNNetOut);

int main(int argc, char *argv[])
{
    if (argc != 5){
        printf("Help:\n");
        printf("Tool to generate diagram from caffe prototxt.\n");
        printf("Usage:\n");
        printf("caffeDiagram  <prototxt> <width> <height> <channel>\n");
        return -1;
    }

    NNet nnet(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    parse(argv[1], &nnet);
    nnet.draw(OUTPUT);

    printf("output saved in %s\n", OUTPUT);
    return 0;
}
