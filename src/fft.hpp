//
// Created by Mikhail Shkarubski on 24.10.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_FFT_H
#define FOURIER_PARALLELIZATION_FFT_H

#include "ShaderExecutor.hpp"
#include "tools.hpp"
#include <omp.h>

#define MASTER_THREAD 0
#define NUM_THREADS 8
#define ENABLE_PARALLEL_FOR_CONDITION 32 // Empirically obtained number

void fftshift(FT::DCVector& x);
void fftshift2d(FT::DCImage& image);
cv::Mat pad(cv::Mat& input, int rows, int cols, int value);
cv::Mat roi(cv::Mat& src, int x, int y, int width, int height);

namespace SR {
    void fft(FT::DCVector&x, bool inverse);
    void fft2d(FT::DCImage& image, bool inverse);
    cv::Mat conv2d(cv::Mat& image, cv::Mat& kernel);
    cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel);
}

namespace MT {
    void fft(FT::DCVector& x, bool inverse, const std::unique_ptr<FFTExecutor>&shader_executor);
    void fft2d(FT::DCImage& image, bool inverse);
    cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel);
}

namespace OMP {
    void fft(FT::DCVector& x, bool inverse);
    void fft2d(FT::DCImage& image, bool inverse);
    cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel);
}


#endif //FOURIER_PARALLELIZATION_FFT_H
