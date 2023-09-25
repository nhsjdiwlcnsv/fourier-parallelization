//
// Created by Mikhail Shkarubski on 17.09.23.
//
#include <string>
#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


Image::Image(const char *filename) {
    if (read(filename)) {
        std::cout << "Read " << filename << std::endl;
        size = w * h * channels;
    } else {
        std::cout << "Failed to read " << filename << std::endl;
    }
}

Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
    size = w * h * channels;
    data = new uint8_t[size];
}

Image::Image(const Image& img) : Image(img.w, img.h, img.channels) {
    memcpy(data, img.data, size);
}

Image::~Image() {
    stbi_image_free(data);
}

bool Image::read(const char *filename) {
    data = stbi_load(filename, &w, &h, &channels, 0);
    return data != nullptr;
}

bool Image::write(const char *filename) {
    ImageType type = getFileType(filename);
    int success;

    switch (type) {
        case PNG:
            success = stbi_write_png(filename, w, h, channels, data, w * channels);
            break;
        case JPG:
            success = stbi_write_jpg(filename, w, h, channels, data, 100);
            break;
        case BMP:
            success = stbi_write_bmp(filename, w, h, channels, data);
            break;
        case TGA:
            success = stbi_write_tga(filename, w, h, channels, data);
            break;
    }

    return success;
}

ImageType Image::getFileType(const char *filename) {
    const char* extension = strrchr(filename, '.');

    if (extension == nullptr)
        return PNG;

    if (strcmp(extension, ".png") == 0)
        return PNG;
    else if (strcmp(extension, ".jpg") == 0)
        return JPG;
    else if (strcmp(extension, ".bmp") == 0)
        return BMP;
    else if (strcmp(extension, ".tga") == 0)
        return TGA;

    return PNG;
}

Image &Image::grayscaleAvg() {
    if (channels < 3)
        std::cout << "Image has less than 3 channels, is already grayscale";
    else {
        for (int i = 0; i < size; i += channels) {
            int gray = (data[i] + data[i + 1] + data[i + 2]) / 3;
            memset(data + i, gray, 3);
        }
    }

    return *this;
}

Image &Image::grayscaleLum() {
    if (channels < 3)
        std::cout << "Image has less than 3 channels, is already grayscale" << std::endl;
    else {
        for (int i = 0; i < size; i += channels) {
            int gray = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];
            memset(data + i, gray, 3);
        }
    }

    return *this;
}

Image &Image::colorMask(float r, float g, float b) {
    if (channels < 3)
        std::cerr << "Image is required to have at least 3 channels!" << std::endl;
    else {
        for (int i = 0; i < size; i += channels) {
            data[i] *= r; data[i + 1] *= g; data[i + 2] *= b;
        }
    }

    return *this;
}

Image &Image::stdConvolveClampTo0(uint8_t channel, uint32_t ker_w, uint32_t ker_h, const double *ker, uint32_t cr, uint32_t cc) {
    uint8_t new_data[w * h];
    uint64_t center = cr * ker_w + cc;

    for (uint64_t k = channel; k < size; k += channels) {
        double sum = 0;

        for (long i = -((long) cr); i < (long) ker_h - cr; ++i) {
            long row = ((long) k / channels) / w - i;
            if (row < 0 || row > h - 1)
                continue;

            for (long j = -((long) cc); j < (long) ker_w - cc; ++j) {
                long col = ((long) k / channels) % w - j;
                if (col < 0 || col > w - 1)
                    continue;

                sum += ker[center + i * (long) ker_w + j] * data[(row * w + col) * channels + channel];
            }
        }

        new_data[k / channels] = (uint8_t)(round(sum));
    }

    for (uint64_t k = channel; k < size; k += channels)
        data[k] = new_data[k / channels];


    return *this;
}

