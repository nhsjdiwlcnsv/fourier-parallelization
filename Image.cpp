//
// Created by Mikhail Shkarubski on 17.09.23.
// The majority of the code can be found at https://www.youtube.com/playlist?list=PLG5M8QIx5lkzdGkdYQeeCK__As6sI2tOY
//
#include <string>
#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// All image-specific constants
#define COLOR_RGB2LUMRED 0.2126
#define COLOR_RGB2LUMGREEN 0.7152
#define COLOR_RGB2LUMBLUE 0.0722


// Image constructor using filename
Image::Image(const char *filename) : w(0), h(0), channels(0) {
    if (read(filename)) {
        std::cout << "Read " << filename << std::endl;
        size = w * h * channels;
    } else {
        std::cout << "Failed to read " << filename << std::endl;
    }
}

// Blank (black) image constructor
Image::Image(int w, int h, int channels) : w(w), h(h), channels(channels) {
    size = w * h * channels;
    data = new uint8_t[size];
}

// Copy constructor (initializes Image fields and copies memory
// into the data of the image being created
Image::Image(const Image& img) : Image(img.w, img.h, img.channels) {
    memcpy(data, img.data, size);
}

// Image destructor (calls standard stbi function for freeing data)
Image::~Image() {
    stbi_image_free(data);
}

bool Image::read(const char *filename) {
    data = stbi_load(filename, &w, &h, &channels, 0);
    return data != nullptr;
}

bool Image::write(const char *filename) const {
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

    if (!strcmp(extension, ".png"))
        return PNG;
    else if (!strcmp(extension, ".jpg"))
        return JPG;
    else if (!strcmp(extension, ".bmp"))
        return BMP;
    else if (!strcmp(extension, ".tga"))
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
            int gray = COLOR_RGB2LUMRED * data[i] + COLOR_RGB2LUMGREEN * data[i + 1] + COLOR_RGB2LUMBLUE * data[i + 2];
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
    uint8_t new_data[w * h]; // new data for a single image channel
    uint64_t center = cr * ker_w + cc; // flattened coordinate of kernel center

    for (uint64_t k = channel; k < size; k += channels) { // iterating through all pixels channel-wise
        double sum = 0; // sum accumulates convolution result

        // starting from the middle row of the kernel, i represents kernel row position
        // starting from the middle row of the kernel, j represents kernel column position
        // indices are taken with minus in order to perform convolution, not cross-correlation
        for (long i = -((long) cr); i < (long) ker_h - cr; ++i) {
            long row = ((long) k / channels) / w; // image row of current pixel
            long row_offset = row - i; // row of the image pixel corresponding to kernel pixel

            if (row_offset < 0 || row_offset > h - 1)
                continue;

            for (long j = -((long) cc); j < (long) ker_w - cc; ++j) {
                long col = ((long) k / channels) % w; // image column of current pixel
                long col_offset = col - j; // column of the image pixel corresponding to kernel pixel

                if (col_offset < 0 || col_offset > w - 1)
                    continue;

                sum += ker[center + i * (long) ker_w + j] * data[(row_offset * w + col_offset) * channels + channel];
            }
        }

        new_data[k / channels] = (uint8_t)(round(sum));
    }

    for (uint64_t k = channel; k < size; k += channels)
        data[k] = new_data[k / channels];


    return *this;
}

