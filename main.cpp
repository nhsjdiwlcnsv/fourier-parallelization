#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"

int main() {
    Image test("/Users/mishashkarubski/CLionProjects/Fourier/test.jpg");

    test.colorMask(0.86, 1, 0.4);

    test.write("/Users/mishashkarubski/CLionProjects/Fourier/test_greeny.png");

    return 0;
}
