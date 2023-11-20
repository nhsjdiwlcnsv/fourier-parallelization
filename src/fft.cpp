//
// Created by Mikhail Shkarubski on 24.10.23.
//

#include "fft.hpp"

// ========================= NAMESPACE SERIAL (SRL) =========================
void SR::fft(FT::DCVector& x, const bool inverse) {
    const auto N = x.size();

    if (N <= 1)
        return;

    const double THETA = (inverse ? 2.0 : -2.0) * M_PI / static_cast<double>(N);

    FT::DCVector even(N / 2), odd(N / 2);

    for (auto i = 0; i < N / 2; ++i) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }

    fft(even, inverse);     fft(odd, inverse);

    for (int k = 0; k < N / 2; ++k) {
        FT::DComplex t = std::polar(1.0, THETA * k) * odd[k];

        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;

        if (inverse) {
            x[k] /= 2;
            x[k + N / 2] /= 2;
        }
    }
}

void SR::fftshift(FT::DCArray& x) {
    const auto N = x.size();
    const auto halfN = N / 2;

    FT::DCArray temp(halfN);
    temp = x[std::slice(0, halfN, 1)];
    x[std::slice(0, halfN, 1)] = x[std::slice(halfN, N, 1)];
    x[std::slice(halfN, N, 1)] = temp;
}

void SR::fft2d(FT::DCImage& image, const bool inverse) {
    // Apply FFT along rows
    for (auto &image_row: image)
        fft(image_row, inverse);

    // Create temporary array to store columns
    FT::DCVector col(image.size());
    // Apply FFT along columns
    for (size_t i = 0; i < image.size(); ++i) {
        for (size_t j = 0; j < image.size(); ++j) col[j] = image[j][i];
        fft(col, inverse);

        for (size_t j = 0; j < image.size(); ++j)
            image[j][i] = col[j];
    }
}

void SR::fftshift2d(FT::DCImage& image) {
    const auto rows = image.size(),
               cols = image[0].size();

    for (int i = 0; i < rows; ++i) {
        FT::DCArray row(image[i].data(), cols);
        fftshift(row);

        for (size_t j = 0; j < cols; ++j)
            image[i][j] = row[j];
    }

    FT::DCArray col(rows);
    for (int i = 0; i < rows; ++i) {
        for (size_t j = 0; j < rows; ++j) col[j] = image[j][i];
        fftshift(col);

        for (size_t j = 0; j < rows; ++j)
            image[j][i] = col[j];
    }
}

cv::Mat SR::conv2d(cv::Mat& image, cv::Mat& kernel) {
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

cv::Mat SR::conv2dfft(cv::Mat& image, cv::Mat& kernel) {
    CV_Assert(image.channels() == 1 && kernel.channels() == 1);

    // Pad the image and kernel to the nearest power of 2 for FFT
    int padRows = 2, padCols = 2;

    while (padRows < image.rows) padRows *= 2;
    while (padCols < image.cols) padCols *= 2;

    cv::Mat result(padRows, padCols, image.type());
    cv::Mat padImage = pad(image, padRows, padCols, 0);
    cv::Mat padKernel = pad(kernel, padRows, padCols, 0);

    // Transform image and kernel to frequency domain
    FT::DCImage fftImage(padRows, FT::DCVector(padCols));
    FT::DCImage fftKernel(padRows, FT::DCVector(padCols));

    MatToDCImage(padImage, fftImage);    MatToDCImage(padKernel, fftKernel);

    fft2d(fftImage, false);             fft2d(fftKernel, false);
    // fftshift2d(fftImage);                     fftshift2d(fftKernel);

    // Element-wise multiplication in frequency domain
    for (int i = 0; i < padRows; ++i)
        for (int j = 0; j < padCols; ++j)
            fftImage[i][j] *= fftKernel[i][j];

    // Inverse FFT to get the result back to spatial domain
    fft2d(fftImage, true);
    fftshift2d(fftImage);

    // Crop the result to the original size
    DCImageToMat(fftImage, result);

    result = roi(result, (padCols - image.cols) / 2, (padRows - image.rows) / 2, image.cols, image.rows);

    return result;
}

cv::Mat pad(cv::Mat& input, int rows, int cols, int value) {
    CV_Assert(rows >= input.rows && cols >= input.cols);

    const int topPad = (rows - input.rows) / 2,
              leftPad = (cols - input.cols) / 2;

    cv::Mat paddedImage(rows, cols, input.type(), cv::Scalar(value));
    input.copyTo(paddedImage(cv::Rect(leftPad, topPad, input.cols, input.rows)));

    return paddedImage;
}

cv::Mat roi(cv::Mat& src, const int x, const int y, const int width, const int height) {
    CV_Assert(width <= src.cols && height <= src.rows);
    CV_Assert(x >= 0 && x <= src.cols && y >= 0 && y <= src.rows);

    const cv::Rect roi(x, y, width, height);
    return src(roi);
}


// ========================= NAMESPACE METAL (MTL) =========================

void MT::fft(FT::DCVector& x, const bool inverse, const std::unique_ptr<FFTExecutor>& shader_executor) {
    const auto N = x.size();

    if (N <= 1)
        return;

    auto n_threads = N / 2;
    auto thread_size = N / n_threads;

    FT::FCVector x_float(N);

    while (n_threads >= 1) {
        for (int i = 0; i < n_threads; ++i)
            for (int j = 0; j < thread_size; ++j)
                x_float[i * thread_size + j] = x[i + j * n_threads];

        shader_executor->send_command(x_float.data(), n_threads, thread_size, inverse);

        const auto *x_ptr = shader_executor->exposeXBuffer<FT::FComplex>;

        for (int i = 0; i < n_threads; ++i)
            for (int j = 0; j < thread_size; ++j)
                x[i + j * n_threads] = x_ptr[i * thread_size + j];

        n_threads /= 2;
        thread_size *= 2;
    }

    std::reverse(x.begin() + 1, x.end());
}

void MT::fftshift(FT::DCArray& x) {
}

void MT::fft2d(FT::DCImage& image, const bool inverse) {
    auto const shader_executor = std::make_unique<FFTExecutor>(NS::String::string("fft", NS::ASCIIStringEncoding));

    // Apply FFT along rows
    for (auto & row : image)
        fft(row, inverse, shader_executor);

    // Create temporary array to store columns
    FT::DCVector col(image.size());

    // Apply FFT along columns
    for (size_t i = 0; i < image.size(); ++i) {
        for (size_t j = 0; j < image.size(); ++j) col[j] = image[j][i];

        fft(col, inverse, shader_executor);

        for (size_t j = 0; j < image.size(); ++j) image[j][i] = col[j];
    }
}

void MT::fftshift2d(FT::DCImage& image) {
}

cv::Mat MT::conv2dfft(cv::Mat& image, cv::Mat& kernel) {
    CV_Assert(image.channels() == 1 && kernel.channels() == 1);

    // Pad the image and kernel to the nearest power of 2 for FFT
    int padRows = 2, padCols = 2;

    while (padRows < image.rows) padRows *= 2;
    while (padCols < image.cols) padCols *= 2;

    cv::Mat result(padRows, padCols, image.type());
    cv::Mat padImage = pad(image, padRows, padCols, 0);
    cv::Mat padKernel = pad(kernel, padRows, padCols, 0);

    // Transform image and kernel to frequency domain
    FT::DCImage fftImage(padRows, FT::DCVector(padCols));
    FT::DCImage fftKernel(padRows, FT::DCVector(padCols));

    MatToDCImage(padImage, fftImage);    MatToDCImage(padKernel, fftKernel);

    fft2d(fftImage, false);             fft2d(fftKernel, false);
    // SR::fftshift2d(fftImage);                  SR::fftshift2d(fftKernel);

    // Element-wise multiplication in frequency domain
    for (int i = 0; i < padRows; ++i)
        for (int j = 0; j < padCols; ++j)
            fftImage[i][j] *= fftKernel[i][j];

    // Inverse FFT to get the result back to spatial domain
    fft2d(fftImage, true);
    SR::fftshift2d(fftImage);

    // Crop the result to the original size
    DCImageToMat(fftImage, result);

    result = roi(result, (padCols - image.cols) / 2, (padRows - image.rows) / 2, image.cols, image.rows);

    return result;
}


// ========================= NAMESPACE OpenMP (OMP) =========================

// void OMP::fft(FT::CArray& x, bool inverse) {
// }
//
// void OMP::fftshift(FT::CArray& x) {
// }
//
// void OMP::fft2d(FT::CImage& image, bool inverse) {
// }
//
// void OMP::fftshift2d(FT::CImage& image) {
// }
//
// cv::Mat OMP::conv2d(cv::Mat& image, cv::Mat& kernel) {
// }
//
// cv::Mat OMP::conv2dfft(cv::Mat& image, cv::Mat& kernel) {
// }
