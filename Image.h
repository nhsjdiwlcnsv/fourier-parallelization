//
// Created by Mikhail Shkarubski on 17.09.23.
//
#include <iostream>

#ifndef FOURIER_IMAGE_H
#define FOURIER_IMAGE_H


enum ImageType {
    PNG,
    JPG,
    BMP,
    TGA,
};


struct Image {
    uint8_t *data = nullptr;
    size_t size = 0;
    int w;
    int h;
    int channels;

    explicit Image(const char *filename);
    Image(int w, int h, int channels);
    Image(const Image &img);
    ~Image();

    bool read(const char *filename);
    bool write (const char *filename);

    ImageType getFileType(const char *filename);

    Image &grayscaleAvg();
    Image &grayscaleLum();
    Image &colorMask(float r, float g, float b);
    Image &stdConvolveClampTo0(uint8_t channel, uint32_t ker_w, uint32_t ker_h, const double *ker, uint32_t cr, uint32_t cc);
};


#endif //FOURIER_IMAGE_H
