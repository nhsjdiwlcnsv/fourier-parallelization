//
// Created by Mikhail Shkarubski on 24.10.23.
//

#include "fft.hpp"

void fft(FT::CArray& x, const bool inverse) {
    const int N = x.size();
    const double THETA = (inverse ? 2.0 : -2.0) * M_PI / N;

    if (N <= 1)
        return;

    FT::CArray even = x[std::slice(0, N / 2, 2)],
               odd = x[std::slice(1, N / 2, 2)];

    fft(even, inverse);     fft(odd, inverse);

    for (int k = 0; k < N / 2; ++k) {
        FT::Complex t = std::polar(1.0, THETA * k) * odd[k];

        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;

        if (inverse) {
            x[k] /= 2;
            x[k + N / 2] /= 2;
        }
    }
}

void fftshift(FT::CArray& x) {
    const int N = x.size();
    const int halfN = N / 2;

    FT::CArray temp(halfN);
    temp = x[std::slice(0, halfN, 1)];
    x[std::slice(0, halfN, 1)] = x[std::slice(halfN, N, 1)];
    x[std::slice(halfN, N, 1)] = temp;
}

void fft2d(FT::CImage& image, const bool inverse) {
    // Apply FFT along rows
    for (auto &image_row: image) {
        FT::CArray row(image_row.data(), image_row.size());
        fft(row, inverse);

        for (size_t j = 0; j < image_row.size(); ++j)
            image_row[j] = row[j];
    }

    // Create temporary array to store columns
    FT::CArray col(image.size());
    // Apply FFT along columns
    for (size_t i = 0; i < image.size(); ++i) {
        for (size_t j = 0; j < image[0].size(); ++j) col[j] = image[j][i];
        fft(col, inverse);

        for (size_t j = 0; j < image[0].size(); ++j)
            image[j][i] = col[j];
    }
}

void fftshift2d(FT::CImage& image) {
    const int rows = image.size(),
              cols = image[0].size();

    for (int i = 0; i < rows; ++i) {
        FT::CArray row(image[i].data(), cols);
        fftshift(row);

        for (size_t j = 0; j < cols; ++j)
            image[i][j] = row[j];
    }

    FT::CArray col(rows);
    for (int i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) col[j] = image[j][i];
        fftshift(col);

        for (size_t j = 0; j < cols; ++j)
            image[j][i] = col[j];
    }
}

cv::Mat pad(cv::Mat& input, int rows, int cols, int value) {
    CV_Assert(rows >= input.rows && cols >= input.cols);

    int topPad = (rows - input.rows) / 2,
        leftPad = (cols - input.cols) / 2;

    cv::Mat paddedImage(rows, cols, input.type(), cv::Scalar(value));
    input.copyTo(paddedImage(cv::Rect(leftPad, topPad, input.cols, input.rows)));

    return paddedImage;
}

cv::Mat roi(cv::Mat& src, int x, int y, int width, int height) {
    CV_Assert(width <= src.cols && height <= src.rows);
    CV_Assert(x > 0 && x <= src.cols && y > 0 && y <= src.rows);

    cv::Rect roi(x, y, width, height);
    return src(roi);
}

cv::Mat conv2d(cv::Mat& image, cv::Mat& kernel) {
    CV_Assert(image.channels() == 1 && kernel.channels() == 1);

    cv::Mat result(image.size(), image.type(), cv::Scalar(0));

    const int kernelCenterX = kernel.cols / 2,
              kernelCenterY = kernel.rows / 2;

    for (int i = kernelCenterY; i < image.rows - kernelCenterY; ++i) {
        for (int j = kernelCenterX; j < image.cols - kernelCenterX; ++j) {
            double sum = 0.0;

            for (int m = -kernelCenterY; m <= kernelCenterY; ++m)
                for (int n = -kernelCenterX; n <= kernelCenterX; ++n)
                    sum += static_cast<double>(image.at<uchar>(i + m, j + n)) * kernel.at<double>(m + kernelCenterY, n + kernelCenterX);

            result.at<uchar>(i, j) = cv::saturate_cast<uchar>(sum);
        }
    }

    return result;
}

cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel) {
    CV_Assert(image.channels() == 1 && kernel.channels() == 1);

    // Pad the image and kernel to the nearest power of 2 for FFT
    int padRows = 2, padCols = 2;

    while (padRows < image.rows) padRows *= 2;
    while (padCols < image.cols) padCols *= 2;

    cv::Mat result(padRows, padCols, image.type());
    cv::Mat padImage = pad(image, padRows, padCols, 0);
    cv::Mat padKernel = pad(kernel, padRows, padCols, 0);

    // Transform image and kernel to frequency domain
    FT::CImage fftImage(padRows, FT::CVector(padCols));
    FT::CImage fftKernel(padRows, FT::CVector(padCols));

    MatToCImage(padImage, fftImage);    MatToCImage(padKernel, fftKernel);

    fft2d(fftImage, false);             fft2d(fftKernel, false);
    // fftshift2d(fftImage);               fftshift2d(fftKernel);


    // Element-wise multiplication in frequency domain
    for (int i = 0; i < padRows; ++i)
        for (int j = 0; j < padCols; ++j)
            fftImage[i][j] *= fftKernel[i][j];

    // Inverse FFT to get the result back to spatial domain
    fft2d(fftImage, true);
    fftshift2d(fftImage);

    // Crop the result to the original size
    CImageToMat(fftImage, result);

    result = roi(result, (padCols - image.cols) / 2, (padRows - image.rows) / 2, image.cols, image.rows);

    return result;
}