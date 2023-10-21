#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Image.h"

int main() {
    double emboss[] = {
            -2 / 9.0, -1 / 9.0, 0.0,
            -1 / 9.0, 1 / 9.0, 1 / 9.0,
            0.0, 1 / 9.0, 2 / 9.0
    };
    double gaussian[] = {
            1 / 273.0, 4 / 273.0, 7 / 273.0, 4 / 273.0, 1 / 273.0,
            4 / 273.0, 16 / 273.0, 26 / 273.0, 16 / 273.0, 4 / 273.0,
            7 / 273.0, 26 / 273.0, 41 / 273.0, 26 / 273.0, 7 / 273.0,
            4 / 273.0, 16 / 273.0, 26 / 273.0, 16 / 273.0, 4 / 273.0,
            1 / 273.0, 4 / 273.0, 7 / 273.0, 4 / 273.0, 1 / 273.0,
    };
    double canny_h[] = {
            -1, -2, -1,
            0, 0, 0,
            1, 2, 1
    };
    double canny_v[] = {
            -1, 0, 1,
            -2, 0, 2,
            -1, 0, 1
    };

    Image test("/Users/mishashkarubski/CLionProjects/Fourier/public/lenna.png");
    test.grayscaleLum();

    Filter canny_h_1(canny_h, 3, 3, 1, 1);

    test.stdConvolveClampTo0(0, 3, 3, canny_v, 1, 1);
    test.stdConvolveClampTo0(1, 3, 3, canny_v, 1, 1);
    test.stdConvolveClampTo0(2, 3, 3, canny_v, 1, 1);

    test.stdConvolveClampTo0(0, canny_h_1);
    test.stdConvolveClampTo0(1, canny_h_1);
    test.stdConvolveClampTo0(2, canny_h_1);

    test.stdConvolveClampTo0(0, 5, 5, gaussian, 2, 2);
    test.stdConvolveClampTo0(1, 5, 5, gaussian, 2, 2);
    test.stdConvolveClampTo0(2, 5, 5, gaussian, 2, 2);

    test.write("/Users/mishashkarubski/CLionProjects/Fourier/public/filtered.png");

    return 0;
}
