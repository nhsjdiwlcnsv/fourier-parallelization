//
// Created by Mikhail Shkarubski on 24.10.23.
//

#include "fft.hpp"

// ===================================== FFT HELPER FUNCTIONS =====================================


void fftshift(FT::DCVector& x) {
    const auto N = x.size();

    std::ranges::rotate(x, x.begin() + N / 2);
}

void fftshift2d(FT::DCImage& image) {
    const auto rows = image.size();

    for (auto & row : image)
        fftshift(row);

    FT::DCVector col(rows);
    for (int i = 0; i < rows; ++i) {
        for (size_t j = 0; j < rows; ++j) col[j] = image[j][i];

        fftshift(col);

        for (size_t j = 0; j < rows; ++j) image[j][i] = col[j];
    }
}

cv::Mat pad(cv::Mat& input, const int rows, const int cols, const int value) {
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


// ==================================== NAMESPACE SERIAL (SRL) ====================================


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



// ===================================== NAMESPACE METAL (MTL) ====================================


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

        const auto *x_ptr = shader_executor->exposeXBuffer<FT::FComplex>();

        for (int i = 0; i < n_threads; ++i)
            for (int j = 0; j < thread_size; ++j)
                x[i + j * n_threads] = x_ptr[i * thread_size + j];

        n_threads /= 2;
        thread_size *= 2;
    }

    std::reverse(x.begin() + 1, x.end());
}

void MT::fft2d(FT::DCImage& image, const bool inverse) {
    NS::Error                 *error                 = nullptr;
    MTL::Device               *device                = MTL::CreateSystemDefaultDevice();
    MTL::Library              *library               = device->newLibrary(NS::String::string("/Users/mishashkarubski/CLionProjects/fourier-parallelization/metal-library/library.metallib", NS::ASCIIStringEncoding), &error);
    MTL::Function             *function              = library->newFunction(NS::String::string("fft2d", NS::ASCIIStringEncoding));
    MTL::CommandQueue         *command_queue         = device->newCommandQueue();
    MTL::ComputePipelineState *pipeline_state        = device->newComputePipelineState(function, &error);

    library->release();
    function->release();

    const size_t nRows = image.size();
    const size_t nCols = image[0].size();

    FT::FCVector x_flattened(nRows * nCols);

    for (int i = 0; i < nRows; ++i)
        for (int j = 0; j < nCols; ++j)
            x_flattened[i * nCols + j] = static_cast<FT::FComplex>(image[i][j]);

    MTL::Buffer *x_buffer                            = device->newBuffer(x_flattened.data(), nRows * nCols * sizeof(FT::FComplex), MTL::StorageModeShared);
    MTL::Buffer *n_rows_buffer                       = device->newBuffer(&nRows, sizeof(size_t), MTL::StorageModeShared);
    MTL::Buffer *n_cols_buffer                       = device->newBuffer(&nCols, sizeof(size_t), MTL::StorageModeShared);
    MTL::Buffer *inv_buffer                          = device->newBuffer(&inverse, sizeof(bool), MTL::StorageModeShared);
    MTL::Buffer *column_buffer                       = device->newBuffer(nCols * nRows * sizeof(FT::FComplex), MTL::StorageModeShared);
    MTL::Buffer *odd_buffer                          = device->newBuffer(nCols * nRows * sizeof(FT::FComplex) / 2, MTL::StorageModeShared);
    MTL::Buffer *even_buffer                         = device->newBuffer(nCols * nRows * sizeof(FT::FComplex) / 2, MTL::StorageModeShared);

    auto *command_buffer                             = command_queue->commandBuffer();
    auto *command_encoder                            = command_buffer->computeCommandEncoder();

    command_encoder->setBuffer(x_buffer,      0, 0);
    command_encoder->setBuffer(n_rows_buffer, 0, 1);
    command_encoder->setBuffer(n_cols_buffer, 0, 2);
    command_encoder->setBuffer(inv_buffer,    0, 3);
    command_encoder->setBuffer(column_buffer, 0, 4);
    command_encoder->setBuffer(odd_buffer,    0, 5);
    command_encoder->setBuffer(even_buffer,   0, 6);

    command_encoder->setComputePipelineState(pipeline_state);

    const NS::UInteger maxThreadsPerGroup        = std::min(std::max(nRows, nCols), pipeline_state->maxTotalThreadsPerThreadgroup());
    const MTL::Size threadsPerGroup              = MTL::Size::Make(maxThreadsPerGroup, 1, 1);
    const MTL::Size threadsPerGrid               = MTL::Size::Make(maxThreadsPerGroup, 1, 1);

    command_encoder->dispatchThreads(threadsPerGrid, threadsPerGroup);
    command_encoder->endEncoding();

    command_buffer->commit();
    command_buffer->waitUntilCompleted();

    const auto *x_data = static_cast<FT::FComplex*>(x_buffer->contents());
    const auto *odd_data = static_cast<FT::FComplex*>(x_buffer->contents());

    for (int i = 0; i < nRows; ++i)
        for (int j = 0; j < nCols; ++j)
            image[i][j] = x_data[i * nCols + j];

    error->release();
    device->release();
    command_queue->release();
    pipeline_state->release();

    x_buffer->release();
    n_rows_buffer->release();
    n_cols_buffer->release();
    inv_buffer->release();
    column_buffer->release();
    odd_buffer->release();
    even_buffer->release();

    // auto const shader_executor = std::make_unique<FFTExecutor>(NS::String::string("fft", NS::ASCIIStringEncoding));
    //
    // Apply FFT along rows
    // for (auto & row : image) {
    //     fft(row, inverse, shader_executor);
    // }
    //
    // // Create temporary array to store columns
    // FT::DCVector col(image.size());
    //
    // // Apply FFT along columns
    // for (size_t i = 0; i < image.size(); ++i) {
    //     for (size_t j = 0; j < image.size(); ++j) col[j] = image[j][i];
    //
    //     // auto const shader_executor = std::make_unique<FFTExecutor>(NS::String::string("fft", NS::ASCIIStringEncoding));
    //     fft(col, inverse, shader_executor);
    //
    //     for (size_t j = 0; j < image.size(); ++j) image[j][i] = col[j];
    // }
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

    fft2d(fftImage, false);              fft2d(fftKernel, false);

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



// ==================================== NAMESPACE OpenMP (OMP) ====================================


void OMP::fft(FT::DCVector& x, const bool inverse) {
    const auto N = x.size(),
               HN = N / 2;

    if (N <= 1)
        return;

    const double THETA = (inverse ? 2.0 : -2.0) * M_PI / static_cast<double>(N);

    FT::DCVector even(HN), odd(HN);

    #pragma omp parallel for schedule(static) if (HN >= ENABLE_PARALLEL_FOR_CONDITION)
    for (auto i = 0; i < HN; ++i) {
        even[i] = x[i * 2];
        odd[i] = x[i * 2 + 1];
    }

    #pragma omp parallel sections
    {
        #pragma omp section
        fft(even, inverse);

        #pragma omp section
        fft(odd, inverse);
    }

    #pragma omp parallel for schedule(static) if (HN >= ENABLE_PARALLEL_FOR_CONDITION)
    for (int k = 0; k < HN; ++k) {
        FT::DComplex t = std::polar(1.0, THETA * k) * odd[k];

        x[k] = even[k] + t;
        x[k + N / 2] = even[k] - t;

        if (inverse) {
            x[k] /= 2;
            x[k + N / 2] /= 2;
        }
    }
}

void OMP::fft2d(FT::DCImage& image, const bool inverse) {
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

cv::Mat OMP::conv2dfft(cv::Mat& image, cv::Mat& kernel) {
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

    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            MatToDCImage(padImage, fftImage);

            #pragma omp section
            MatToDCImage(padKernel, fftKernel);
        }

        #pragma omp barrier

        #pragma omp sections
        {
            #pragma omp section
            fft2d(fftImage, false);

            #pragma omp section
            fft2d(fftKernel, false);
        }
    }

    // Element-wise multiplication in frequency domain
    #pragma omp parallel for collapse(2)
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
