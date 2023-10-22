// 2D Image Convolution
#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <chrono>

#define freqMat std::vector<std::vector<std::complex<double>>>
#define freqVec std::vector<std::complex<double>>
#define cmplx std::complex<double>

template <class T, class U>
std::vector<std::vector<T>> Conv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();
    int kSize = kernel.size();
    int kCenter = kSize / 2;

    std::vector<std::vector<T>> result(imgHeight, std::vector<T>(imgWidth, 0));

    for (int i = kCenter; i < imgHeight - kCenter; ++i) {
        for (int j = kCenter; j < imgWidth - kCenter; ++j) {
            T sum = 0;

            for (int k = -kCenter; k <= kCenter; ++k)
                for (int l = -kCenter; l <= kCenter; ++l)
                    sum += image[i + k][j + l] * kernel[k + kCenter][l + kCenter];

            result[i][j] = sum;
        }
    }

    return result;
}

template <class T>
freqMat fft2d(const std::vector<std::vector<T>>& image) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();

    // Convert image to complex values
    freqMat imgTransformed(imgHeight, freqVec(imgWidth));
    for (size_t i = 0; i < imgHeight; ++i)
        for (size_t j = 0; j < imgWidth; ++j)
            imgTransformed[i][j] = cmplx(image[i][j], 0);

    // Apply fft2d to each row
    for (size_t i = 0; i < imgHeight; ++i) {
        freqVec row = imgTransformed[i];
        freqVec transformedRow(imgWidth);

        for (size_t j = 0; j < imgWidth; ++j) {
            cmplx sum(0, 0);
            for (size_t k = 0; k < imgWidth; ++k)
                sum += row[k] * std::exp(cmplx(0, -2 * M_PI * j * k / imgWidth));

            transformedRow[j] = sum;
        }

        imgTransformed[i] = transformedRow;
    }

    // Apply fft2d to each column
    for (size_t j = 0; j < imgWidth; ++j) {
        freqVec column(imgHeight);
        freqVec transformedColumn(imgHeight);

        for (size_t i = 0; i < imgHeight; ++i)
            column[i] = imgTransformed[i][j];

        for (size_t i = 0; i < imgHeight; ++i) {
            cmplx sum(0, 0);
            for (size_t k = 0; k < imgHeight; ++k)
                sum += column[k] * std::exp(cmplx(0, -2 * M_PI * i * k / imgHeight));

            transformedColumn[i] = sum;
        }

        for (size_t i = 0; i < imgHeight; ++i)
            imgTransformed[i][j] = transformedColumn[i];
    }

    return imgTransformed;
}


template <class T>
std::vector<std::vector<T>> ifft2d(const freqMat& image) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();

    freqMat imgITransformed(imgHeight, freqVec(imgWidth));
    std::vector<std::vector<T>> result(imgHeight, std::vector<T>(imgWidth));

    // Apply ifft2d to each column
    for (size_t j = 0; j < imgWidth; ++j) {
        freqVec column(imgHeight);
        freqVec columnITransformed(imgHeight);

        for (size_t i = 0; i < imgHeight; ++i)
            column[i] = image[i][j];

        for (size_t i = 0; i < imgHeight; ++i) {
            cmplx sum(0, 0);
            for (size_t k = 0; k < imgHeight; ++k)
                sum += column[k] * std::exp(cmplx(0, 2 * M_PI * i * k / imgHeight));

            columnITransformed[i] = sum / (double) imgHeight;
        }

        for (size_t i = 0; i < imgHeight; ++i)
            imgITransformed[i][j] = columnITransformed[i];
    }

    // Apply ifft2d to each row
    for (size_t i = 0; i < imgHeight; ++i) {
        freqVec row = imgITransformed[i];
        freqVec rowITransformed(imgWidth);

        for (size_t j = 0; j < imgWidth; ++j) {
            cmplx sum(0, 0);
            for (size_t k = 0; k < imgWidth; ++k)
                sum += row[k] * std::exp(cmplx(0, 2 * M_PI * j * k / imgWidth));

            rowITransformed[j] = sum / (double) imgWidth;
        }

        imgITransformed[i] = rowITransformed;
    }

    // Convert the values to real
    for (size_t i = 0; i < imgWidth; ++i)
        for (size_t j = 0; j < imgHeight; ++j)
            result[i][j] = static_cast<T>(std::real(imgITransformed[i][j]));

    return result;
}

template <class T, class U>
std::vector<std::vector<T>> ftConv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();
    int kSize = kernel.size();
    int kCenter = kSize / 2;

    freqMat imgTransformed = fft2d<T>(image);
    freqMat kernelTransformed = fft2d<U>(kernel);
    freqMat resultTransformed(imgHeight, freqVec(imgWidth));

    for (size_t i = 0; i < imgHeight; ++i) {
        for (size_t j = 0; j < imgWidth; ++j) {
            cmplx sum(0, 0);

            for (int k = 0; k < kSize; ++k) {
                for (int l = 0; l < kSize; ++l) {
                    size_t imgY = (i + kCenter - k + imgHeight) % imgHeight;
                    size_t imgX = (j + kCenter - l + imgWidth) % imgWidth;

                    if (imgY < 0 || imgY >= imgHeight || imgX < 0 || imgX >= imgWidth)
                        continue;

                    sum += imgTransformed[imgY][imgX] * kernelTransformed[k][l];
                }
            }

            resultTransformed[i][j] = sum;
        }
    }

    std::vector<std::vector<T>> result = ifft2d<T>(resultTransformed);

    return result;
}

int main() {
    std::vector<std::vector<int>> image = {{1, 2, 3, 4, 5},
                                           {6, 7, 8, 9, 10},
                                           {11, 12, 13, 14, 15},
                                           {16, 17, 18, 19, 20},
                                           {21, 22, 23, 24, 25}};
    std::vector<std::vector<int>> kernel = {{-1, 0, 1},
                                            {-2, 0, 2},
                                            {-1, 0, 1}};

    auto beg1 = std::chrono::high_resolution_clock::now();
    auto result1 = Conv(image, kernel);
    auto end1 = std::chrono::high_resolution_clock::now();

    auto beg2 = std::chrono::high_resolution_clock::now();
    auto result2 = ftConv(image, kernel);
    auto end2 = std::chrono::high_resolution_clock::now();

    auto nImage = ifft2d<int>(fft2d<int>(image));

    for (const auto& row : nImage) {
        for (auto pixel : row)
            std::cout << pixel << " ";

        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end1 - beg1).count() << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end2 - beg2).count() << std::endl;
    std::cout << std::endl;

    for (const auto& row : result1) {
        for (auto pixel : row)
            std::cout << (double) pixel << " ";

        std::cout << std::endl;
    }

    std::cout << std::endl;

    for (const auto& row : result2) {
        for (auto pixel : row)
            std::cout << (double) pixel << " ";

        std::cout << std::endl;
    }

    return 0;
}
