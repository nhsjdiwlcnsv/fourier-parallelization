//
// Created by Mikhail Shkarubski on 24.10.23.
//

#include "fft.hpp"

// ========================= NAMESPACE SERIAL (SRL) =========================

void SR::fft(FT::DCArray& x, const bool inverse) {
    const auto N = x.size();

    if (N <= 1)
        return;

    const double THETA = (inverse ? 2.0 : -2.0) * M_PI / static_cast<double>(N);

    FT::DCArray even = x[std::slice(0, N / 2, 2)],
                odd = x[std::slice(1, N / 2, 2)];

    fft(even, inverse);     fft(odd, inverse);

    x = FT::DCArray(N);

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
    for (auto &image_row: image) {
        FT::DCArray row(image_row.data(), image_row.size());
        fft(row, inverse);

        for (size_t j = 0; j < image_row.size(); ++j)
            image_row[j] = row[j];
    }

    // Create temporary array to store columns
    FT::DCArray col(image.size());
    // Apply FFT along columns
    for (size_t i = 0; i < image.size(); ++i) {
        for (size_t j = 0; j < image[0].size(); ++j) col[j] = image[j][i];
        fft(col, inverse);

        for (size_t j = 0; j < image[0].size(); ++j)
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
        for (size_t j = 0; j < cols; ++j) col[j] = image[j][i];
        fftshift(col);

        for (size_t j = 0; j < cols; ++j)
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

    MatToCImage(padImage, fftImage);    MatToCImage(padKernel, fftKernel);

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
    CImageToMat(fftImage, result);

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
    CV_Assert(x > 0 && x <= src.cols && y > 0 && y <= src.rows);

    const cv::Rect roi(x, y, width, height);
    return src(roi);
}


// ========================= NAMESPACE METAL (MTL) =========================

void MT::fft(FT::DCVector& x, const bool inverse) {
    const auto N = x.size();

    if (N <= 1)
        return;

    auto n_threads = N / 2;
    auto thread_size = N / n_threads;

    NS::Error *error = nullptr;
    MTL::Device *device = MTL::CreateSystemDefaultDevice();
    MTL::CommandQueue *command_queue = device->newCommandQueue();

    const NS::String* lib_path = NS::String::string(
        "/Users/mishashkarubski/CLionProjects/fourier-parallelization/metal-library/library.metallib",
        NS::ASCIIStringEncoding);
    MTL::Library *library = device->newLibrary(lib_path, &error);

    const NS::String* function_name = NS::String::string("fft", NS::ASCIIStringEncoding);
    const MTL::Function *function = library->newFunction(function_name);

    const MTL::ComputePipelineState *pipeline_state = device->newComputePipelineState(function, &error);

    while (n_threads >= 1) {
         FT::FCVector x_float(N);

        for (int i = 0; i < n_threads; ++i)
            for (int j = 0; j < thread_size; ++j)
                x_float[i * thread_size + j] = x[i + j * n_threads];

        MTL::Buffer *x_buffer               = device->newBuffer(x_float.data(), N * sizeof(FT::FComplex), MTL::ResourceStorageModeShared);
        MTL::Buffer *th_sz_buffer           = device->newBuffer(&thread_size, sizeof(size_t), MTL::ResourceStorageModeShared);
        MTL::Buffer *inv_buffer             = device->newBuffer(&inverse, sizeof(bool), MTL::ResourceStorageModeShared);
        MTL::Buffer *even_buffer            = device->newBuffer(n_threads * (thread_size / 2) * sizeof(FT::FComplex), MTL::ResourceStorageModeShared);
        MTL::Buffer *odd_buffer             = device->newBuffer(n_threads * (thread_size / 2) * sizeof(FT::FComplex), MTL::ResourceStorageModeShared);

        auto *command_buffer = command_queue->commandBuffer();
        MTL::ComputeCommandEncoder *command_encoder = command_buffer->computeCommandEncoder();

        command_encoder->setComputePipelineState(pipeline_state);

        command_encoder->setBuffer(x_buffer, 0, 0);
        command_encoder->setBuffer(th_sz_buffer, 0, 1);
        command_encoder->setBuffer(inv_buffer, 0, 2);
        command_encoder->setBuffer(even_buffer, 0, 3);
        command_encoder->setBuffer(odd_buffer, 0, 4);

        const MTL::Size threads_per_grid               = MTL::Size::Make(n_threads, 1, 1);
        const NS::UInteger max_threads_per_group       = pipeline_state->maxTotalThreadsPerThreadgroup();
        const MTL::Size threads_per_group              = MTL::Size::Make(max_threads_per_group, 1, 1);

        command_encoder->dispatchThreads(threads_per_grid, threads_per_group);
        command_encoder->endEncoding();

        command_buffer->commit();
        command_buffer->waitUntilCompleted();

        const auto *x_ptr = static_cast<FT::FComplex*>(x_buffer->contents());

        for (int i = 0; i < n_threads; ++i)
            for (int j = 0; j < thread_size; ++j)
                x[i + j * n_threads] = x_ptr[i * thread_size + j];

        n_threads /= 2;
        thread_size *= 2;
    }

    std::reverse(x.begin() + 1, x.end());
}
//
// void MT::fftshift(FT::CArray& x) {
// }
//
void MT::fft2d(FT::DCImage& image, bool inverse) {
    // Apply FFT along rows
    for (auto &image_row: image)
        fft(image_row, inverse);

    // Create temporary array to store columns
    FT::DCVector col(image.size());

    // Apply FFT along columns
    for (size_t i = 0; i < image.size(); ++i) {
        for (size_t j = 0; j < image[0].size(); ++j) col[j] = image[j][i];
        fft(col, inverse);

        for (size_t j = 0; j < image[0].size(); ++j)
            image[j][i] = col[j];
    }
}
//
// void MT::fftshift2d(FT::CImage& image) {
// }
//
// cv::Mat MT::conv2d(cv::Mat& image, cv::Mat& kernel) {
// }
//
// cv::Mat MT::conv2dfft(cv::Mat& image, cv::Mat& kernel) {
// }


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