////
//// Created by Mikhail Shkarubski on 24.10.23.
////
//#include "fft.h"
//
//
//bool fftools::fft(freqVec& x, int sign) {
//    const size_t n = x.size();
//
//    if (n <= 1)
//        return true;
//
//    if (sign != 1 && sign != -1) // basically, if sign = -1 we're dealing inverse FFT
//        return false;
//
//    freqVec xEven(n / 2), xOdd(n / 2);
//
//    for (size_t i = 0; i < n / 2; ++i) {
//        xEven[i] = x[2 * i];
//        xOdd[i] = x[2 * i + 1];
//    }
//
//    fft(xEven, sign);
//    fft(xOdd, sign);
//
//    double angle = sign * 2 * M_PI / n;
//    cmplx w(1), wn(std::cos(angle), std::sin(angle));
//
//    for (size_t i = 0; i < n / 2; ++i) {
//        cmplx t = w * xOdd[i];
//        x[i] = xEven[i] + t;
//        x[i + n / 2] = xEven[i] - t;
//        w *= wn;
//    }
//
//    if (sign == 1) {
//        for (size_t i = 0; i < n; ++i)
//            x[i] /= 2;
//    }
//
//    return true;
//}
//
//template <class T, class U>
//std::vector<std::vector<T>> fftools::Conv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel) {
//    size_t imgHeight = image.size();
//    size_t imgWidth = image[0].size();
//    int kSize = kernel.size();
//    int kCenter = kSize / 2;
//
//    std::vector<std::vector<T>> result(imgHeight, std::vector<T>(imgWidth, 0));
//
//    for (int i = kCenter; i < imgHeight - kCenter; ++i) {
//        for (int j = kCenter; j < imgWidth - kCenter; ++j) {
//            T sum = 0;
//
//            for (int k = -kCenter; k <= kCenter; ++k)
//                for (int l = -kCenter; l <= kCenter; ++l)
//                    sum += image[i + k][j + l] * kernel[k + kCenter][l + kCenter];
//
//            result[i][j] = sum;
//        }
//    }
//
//    return result;
//}
//
//template <class T>
//std::vector<std::vector<T>> fftools::dftshift2d(std::vector<std::vector<T>> kernel, size_t rows, size_t cols) {
//    std::vector<std::vector<T>> kPadded(rows, std::vector<T>(cols, 0));
//
//    size_t kRows = kernel.size(), kCols = kernel[0].size();
//    size_t originY = (rows - kRows) / 2, originX = (cols - kCols) / 2;
//
//    for (int i = 0; i < kRows; i++)
//        for (int j = 0; j < kCols; j++)
//            kPadded[originY + i][originX + j] = kernel[i][j];
//
//    for (int i = 0; i < rows; i++)
//        std::rotate(kPadded[i].begin(), kPadded[i].begin() + cols / 2, kPadded[i].end());
//
//    for (int j = 0; j < cols; j++) {
//        std::vector<double> column(rows);
//        for (int i = 0; i < rows; i++)
//            column[i] = kPadded[i][j];
//
//        std::rotate(column.begin(), column.begin() + rows / 2, column.end());
//        for (int i = 0; i < rows; i++)
//            kPadded[i][j] = column[i];
//    }
//
//    return kPadded;
//}
//
//template <class T>
//freqMat fftools::dft2d(const std::vector<std::vector<T>>& image) {
//    size_t imgHeight = image.size();
//    size_t imgWidth = image[0].size();
//
//    freqMat imgTransformed(imgHeight, freqVec(imgWidth));
//
//    for (size_t i = 0; i < imgHeight; ++i) {
//        freqVec row(imgWidth);
//        for (size_t j = 0; j < imgWidth; ++j)
//            row[j] = cmplx(image[i][j], 0);
//
//        fft(row, 1);
//        imgTransformed[i] = row;
//    }
//
//    for (size_t j = 0; j < imgWidth; j++) {
//        freqVec col(imgWidth);
//        for (size_t i = 0; i < imgHeight; i++)
//            col[j] = imgTransformed[i][j];
//
//        fft(col, 1);
//
//        for (size_t i = 0; i < imgHeight; i++)
//            imgTransformed[i][j] = col[i];
//    }
////
////    // Apply dft2d to each row
////    for (size_t i = 0; i < imgHeight; ++i) {
////        freqVec row = imgTransformed[i];
////        freqVec transformedRow(imgWidth);
////
////        for (size_t j = 0; j < imgWidth; ++j) {
////            cmplx sum(0, 0);
////            for (size_t k = 0; k < imgWidth; ++k)
////                sum += row[k] * std::exp(cmplx(0, -2 * M_PI * j * k / imgWidth));
////
////            transformedRow[j] = sum;
////        }
////
////        imgTransformed[i] = transformedRow;
////    }
////
////    // Apply dft2d to each column
////    for (size_t j = 0; j < imgWidth; ++j) {
////        freqVec column(imgHeight);
////        freqVec transformedColumn(imgHeight);
////
////        for (size_t i = 0; i < imgHeight; ++i)
////            column[i] = imgTransformed[i][j];
////
////        for (size_t i = 0; i < imgHeight; ++i) {
////            cmplx sum(0, 0);
////            for (size_t k = 0; k < imgHeight; ++k)
////                sum += column[k] * std::exp(cmplx(0, -2 * M_PI * i * k / imgHeight));
////
////            transformedColumn[i] = sum;
////        }
////
////        for (size_t i = 0; i < imgHeight; ++i)
////            imgTransformed[i][j] = transformedColumn[i];
////    }
//
//    return imgTransformed;
//}
//
//
//template <class T>
//std::vector<std::vector<std::complex<T>>> fftools::idft2d(const freqMat& image) {
//    size_t imgHeight = image.size();
//    size_t imgWidth = image[0].size();
//
//    freqMat imgITransformed(imgHeight, freqVec(imgWidth));
//    std::vector<std::vector<std::complex<T>>> result(imgHeight, std::vector<std::complex<T>>(imgWidth));
//
//    for (size_t i = 0; i < imgHeight; ++i) {
//        freqVec row = image[i];
//        fft(row, 1);
//        imgITransformed[i] = row;
//    }
//
//    for (size_t j = 0; j < imgWidth; j++) {
//        freqVec col(imgWidth);
//        for (size_t i = 0; i < imgHeight; i++)
//            col[j] = imgITransformed[i][j];
//
//        fft(col, 1);
//
//        for (size_t i = 0; i < imgHeight; i++)
//            imgITransformed[i][j] = col[i];
//    }
//    // Apply idft2d to each column
////    for (size_t j = 0; j < imgWidth; ++j) {
////        freqVec column(imgHeight);
////        freqVec columnITransformed(imgHeight);
////
////        for (size_t i = 0; i < imgHeight; ++i)
////            column[i] = image[i][j];
////
////        for (size_t i = 0; i < imgHeight; ++i) {
////            cmplx sum(0, 0);
////            for (size_t k = 0; k < imgHeight; ++k)
////                sum += column[k] * std::exp(cmplx(0, 2 * M_PI * i * k / imgHeight));
////
////            columnITransformed[i] = sum / (double) imgHeight;
////        }
////
////        for (size_t i = 0; i < imgHeight; ++i)
////            imgITransformed[i][j] = columnITransformed[i];
////    }
////
////    // Apply idft2d to each row
////    for (size_t i = 0; i < imgHeight; ++i) {
////        freqVec row = imgITransformed[i];
////        freqVec rowITransformed(imgWidth);
////
////        for (size_t j = 0; j < imgWidth; ++j) {
////            cmplx sum(0, 0);
////            for (size_t k = 0; k < imgWidth; ++k)
////                sum += row[k] * std::exp(cmplx(0, 2 * M_PI * j * k / imgWidth));
////
////            rowITransformed[j] = sum / (double) imgWidth;
////        }
////
////        imgITransformed[i] = rowITransformed;
////    }
//
//    // Convert the values to real
//    for (size_t i = 0; i < imgWidth; ++i)
//        for (size_t j = 0; j < imgHeight; ++j)
//            result[i][j] = -static_cast<std::complex<T>>(imgITransformed[i][j]);
//
//    return result;
//}
//
//template <class T, class U>
//std::vector<std::vector<std::complex<T>>> fftools::dftConv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel) {
//    size_t imgHeight = image.size(), imgWidth = image[0].size();
//    size_t vPad = (imgHeight - kernel.size()) / 2, hPad = (imgWidth - kernel[0].size()) / 2;
//
//    std::vector<std::vector<U>> kPadded = fftools::dftshift2d<U>(kernel, imgHeight, imgWidth);
//    freqMatT resultPadded = freqMatT(imgHeight, freqVecT(imgWidth, 0));
//
//    freqMat imgTransformed = fftools::dft2d<T>(image);
//    freqMat kTransformed = fftools::dft2d<U>(kPadded);
//    freqMat resultTransformed(imgHeight, freqVec(imgWidth));
//
//    for (size_t i = 0; i < imgHeight; ++i)
//        for (size_t j = 0; j < imgWidth; ++j)
//            resultTransformed[i][j] = imgTransformed[i][j] * kTransformed[i][j];
//
//    freqMatT result = fftools::idft2d<T>(resultTransformed);
//
//    for (size_t i = vPad; i < imgHeight - vPad; ++i)
//        for (size_t j = hPad; j < imgWidth - hPad; ++j)
//            resultPadded[i][j] = result[i][j];
//
//    return resultPadded;
//}
#include "fft.h"

void fft(CArray& x) {
    const size_t N = x.size();

    if (N <= 1)
        return;

    CArray even = x[std::slice(0, N / 2, 2)];
    CArray odd = x[std::slice(1, N / 2, 2)];

    for (size_t i = 0; i < N / 2; ++i) {
        Complex t = std::polar(1.0, -2 * M_PI * i / N) * odd[i];
        x[i] = even[i] + t;
        x[i + N / 2] = even[i] - t;
    }
}

void fft2d(CImage& image) {
    // Apply FFT along rows
    for (auto & image_row : image) {
        CArray row(image_row.data(), image_row.size());
        fft(row);

        for (size_t j = 0; j < image_row.size(); ++j)
            image_row[j] = row[j];
    }

    // Create temporary array to store columns
    CArray temp(image.size());

    // Apply FFT along columns
    for (size_t i = 0; i < image[0].size(); ++i) {
        for (size_t j = 0; j < image.size(); ++j) temp[j] = image[j][i];
        fft(temp);

        for (size_t j = 0; j < image.size(); ++j)
            image[j][i] = temp[j];
    }
}
