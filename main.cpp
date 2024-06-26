#include <cstdio>

#include "imageIo.h"

int main()
{
    auto img = image::Image::load("./starry_night.jpg");
    printf("Width: %d x Height %d\n", img.width(), img.height());

    for (int y = 0; y < 512; y++) {
        for (int x = 0; x < 512; x++) {
            img.at(x, y)->b = 0;
        }
    }

    img.show_blocking();

    return 0;
}
