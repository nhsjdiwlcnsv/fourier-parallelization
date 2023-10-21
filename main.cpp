// 2D Image Convolution
#include <iostream>
#include <vector>
#include <cmath>
#include <complex>

std::vector<std::vector<uint8_t>> Conv(const std::vector<std::vector<int>>& image, const std::vector<std::vector<int>>& kernel) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();
    size_t kSize = kernel.size();
    size_t kCenter = kSize / 2;

    std::vector<std::vector<uint8_t>> result(imgHeight, std::vector<uint8_t>(imgWidth, 0));

    for (size_t i = kCenter; i < imgHeight - kCenter; ++i) {
        for (size_t j = kCenter; j < imgWidth - kCenter; ++j) {
            uint8_t sum = 0;
            for (size_t k = -kCenter; k <= kCenter; ++k) {
                for (size_t l = -kCenter; l <= kCenter; ++l) {
                    sum += image[i + k][j + l] * kernel[k + kCenter][l + kCenter];
                }
            }
            result[i][j] = sum;
        }
    }

    return result;
}

template <class T>
std::vector<std::vector<std::complex<double>>> fft(const std::vector<std::vector<T>>& image) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();

    // Convert image to complex values
    std::vector<std::vector<std::complex<double>>> imgTransformed(imgHeight, std::vector<std::complex<double>>(imgWidth));
    for (size_t i = 0; i < imgHeight; ++i)
        for (size_t j = 0; j < imgWidth; ++j)
            imgTransformed[i][j] = std::complex<double>(image[i][j], 0);

    // Apply fft to each row
    for (size_t i = 0; i < imgHeight; ++i) {
        std::vector<std::complex<double>> row = imgTransformed[i];
        std::vector<std::complex<double>> transformedRow(imgWidth);

        for (size_t j = 0; j < imgWidth; ++j) {
            std::complex<double> sum(0, 0);
            for (size_t k = 0; k < imgWidth; ++k)
                sum += row[k] * std::exp(std::complex<double>(0, -2 * M_PI * j * k / imgWidth));

            transformedRow[j] = sum;
        }

        imgTransformed[i] = transformedRow;
    }

    // Apply fft to each column
    for (size_t j = 0; j < imgWidth; ++j) {
        std::vector<std::complex<double>> column(imgHeight);
        std::vector<std::complex<double>> transformedColumn(imgHeight);

        for (size_t i = 0; i < imgHeight; ++i)
            column[i] = imgTransformed[i][j];

        for (size_t i = 0; i < imgHeight; ++i) {
            std::complex<double> sum(0, 0);
            for (size_t k = 0; k < imgHeight; ++k)
                sum += column[k] * std::exp(std::complex<double>(0, -2 * M_PI * i * k / imgHeight));

            transformedColumn[i] = sum;
        }

        for (size_t i = 0; i < imgHeight; ++i)
            imgTransformed[i][j] = transformedColumn[i];
    }

    return imgTransformed;
}


template <class T>
std::vector<std::vector<T>> ifft(const std::vector<std::vector<std::complex<double>>>& image) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();

    std::vector<std::vector<T>> imgITransformed(imgHeight, std::vector<T>(imgWidth));

    // Apply ifft to each column
    for (size_t j = 0; j < imgWidth; ++j) {
        std::vector<std::complex<double>> column(imgHeight);
        std::vector<std::complex<double>> columnITransformed(imgHeight);

        for (size_t i = 0; i < imgHeight; ++i)
            column[i] = image[i][j];

        for (size_t i = 0; i < imgHeight; ++i) {
            std::complex<double> sum(0, 0);
            for (size_t k = 0; k < imgHeight; ++k)
                sum += column[k] * std::exp(std::complex<double>(0, 2 * M_PI * i * k / imgHeight));

            columnITransformed[i] = sum / (double) imgHeight;
        }

        for (size_t i = 0; i < imgHeight; ++i)
            imgITransformed[i][j] = static_cast<T>(std::real(columnITransformed[i]));
    }

    // Apply ifft to each row
    for (size_t i = 0; i < imgHeight; ++i) {
        std::vector<std::complex<double>> row(imgWidth);
        std::vector<std::complex<double>> rowITransformed(imgWidth);

        for (size_t j = 0; j < imgWidth; ++j)
            row[i] = image[i][j];

        for (size_t j = 0; j < imgWidth; ++j) {
            std::complex<double> sum(0, 0);
            for (size_t k = 0; k < imgWidth; ++k)
                sum += row[k] * std::exp(std::complex<double>(0, 2 * M_PI * j * k / imgWidth));

            rowITransformed[j] = sum / (double) imgWidth;
        }

        for (size_t j = 0; j < imgWidth; ++j)
            imgITransformed[i][j] = static_cast<T>(std::real(rowITransformed[j]));
    }

    return imgITransformed;
}


int main() {
    std::vector<std::vector<uint8_t>> image = {{255, 0, 255},
                                               {0, 255, 0},
                                               {255, 0, 255}};
    std::vector<std::vector<int>> kernel = {{-1, 0, 1},
                                            {-2,  0, 2},
                                            {-1, 0, 1}};

    std::vector<std::vector<std::complex<double>>> transformed_image = fft<uint8_t>(image);
    std::vector<std::vector<std::complex<double>>> transformed_kernel = fft<int>(kernel);

    auto image_result = ifft<uint8_t>(transformed_image);
    auto kernel_result = ifft<uint8_t>(transformed_kernel);

    for (const auto& row : image_result) {
        for (auto pixel : row)
            std::cout << (double) pixel << " ";

        std::cout << std::endl;
    }

    return 0;
}
