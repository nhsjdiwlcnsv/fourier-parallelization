//
// Created by Mikhail Shkarubski on 24.10.23.
//
#include "fft.h"


template <class T, class U>
std::vector<std::vector<T>> fft::Conv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel) {
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
std::vector<std::vector<T>> fft::dftshift2d(std::vector<std::vector<T>> kernel, size_t rows, size_t cols) {
    // Create blank matrix of image size
    std::vector<std::vector<T>> kPadded(rows, std::vector<T>(cols, 0));

    size_t kernelRows = kernel.size();
    size_t kernelCols = kernel[0].size();

    // Calculate the starting position for the kernel
    size_t startRow = (rows - kernelRows) / 2;
    size_t startCol = (cols - kernelCols) / 2;

    // Copy kernel to the center of the matrix
    for (int i = 0; i < kernelRows; i++)
        for (int j = 0; j < kernelCols; j++)
            kPadded[startRow + i][startCol + j] = kernel[i][j];

    // Shift rows
    for (int i = 0; i < rows; i++)
        std::rotate(kPadded[i].begin(), kPadded[i].begin() + cols / 2, kPadded[i].end());

    // Shift columns
    for (int j = 0; j < cols; j++) {
        std::vector<double> column(rows);
        for (int i = 0; i < rows; i++)
            column[i] = kPadded[i][j];

        std::rotate(column.begin(), column.begin() + rows / 2, column.end());
        for (int i = 0; i < rows; i++)
            kPadded[i][j] = column[i];
    }

    return kPadded;
}

template <class T>
freqMat fft::dft2d(const std::vector<std::vector<T>>& image) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();

    // Convert image to complex values
    freqMat imgTransformed(imgHeight, freqVec(imgWidth));
    for (size_t i = 0; i < imgHeight; ++i)
        for (size_t j = 0; j < imgWidth; ++j)
            imgTransformed[i][j] = cmplx(image[i][j], 0);

    // Apply dft2d to each row
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

    // Apply dft2d to each column
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
std::vector<std::vector<std::complex<T>>> fft::idft2d(const freqMat& image) {
    size_t imgHeight = image.size();
    size_t imgWidth = image[0].size();

    freqMat imgITransformed(imgHeight, freqVec(imgWidth));
    std::vector<std::vector<std::complex<T>>> result(imgHeight, std::vector<std::complex<T>>(imgWidth));

    // Apply idft2d to each column
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

    // Apply idft2d to each row
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
            result[i][j] = -static_cast<std::complex<T>>(imgITransformed[i][j]);

    return result;
}

template <class T, class U>
std::vector<std::vector<std::complex<T>>> fft::dftConv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel) {
    size_t imgHeight = image.size(), imgWidth = image[0].size();
    size_t vPad = (imgHeight - kernel.size()) / 2, hPad = (imgWidth - kernel[0].size()) / 2;

    std::vector<std::vector<U>> kPadded = fft::dftshift2d<U>(kernel, imgHeight, imgWidth);
    freqMatT resultPadded = freqMatT(imgHeight, freqVecT(imgWidth, 0));

    freqMat imgTransformed = fft::dft2d<T>(image);
    freqMat kTransformed = fft::dft2d<U>(kPadded);
    freqMat resultTransformed(imgHeight, freqVec(imgWidth));

    for (size_t i = 0; i < imgHeight; ++i)
        for (size_t j = 0; j < imgWidth; ++j)
            resultTransformed[i][j] = imgTransformed[i][j] * kTransformed[i][j];

    freqMatT result = fft::idft2d<T>(resultTransformed);

    for (size_t i = vPad; i < imgHeight - vPad; ++i)
        for (size_t j = hPad; j < imgWidth - hPad; ++j)
            resultPadded[i][j] = result[i][j];

    return resultPadded;
}