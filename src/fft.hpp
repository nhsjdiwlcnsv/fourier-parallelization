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
#define ENABLE_PARALLEL_FOR_CONDITION 64 // Empirically obtained number

namespace SR {
    void fft(FT::DCVector&x, bool inverse);
    void fftshift(FT::DCArray& x);
    void fft2d(FT::DCImage& image, bool inverse);
    void fftshift2d(FT::DCImage& image);
    cv::Mat conv2d(cv::Mat& image, cv::Mat& kernel);
    cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel);
}

namespace MT {
    void fft(FT::DCVector& x, bool inverse, const std::unique_ptr<FFTExecutor>& shader_executor);
    void fftshift(FT::DCArray& x);
    void fft2d(FT::DCImage& image, bool inverse);
    void fftshift2d(FT::DCImage& image);
    cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel);
}

namespace OMP {
    void fft(FT::DCVector& x, bool inverse);
    void fftshift(FT::DCVector& x);
    void fft2d(FT::DCImage& image, bool inverse);
    void fftshift2d(FT::DCImage& image);
    cv::Mat conv2d(cv::Mat& image, cv::Mat& kernel);
    cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel);
}

cv::Mat pad(cv::Mat& input, int rows, int cols, int value);
cv::Mat roi(cv::Mat& src, int x, int y, int width, int height);

#endif //FOURIER_PARALLELIZATION_FFT_H
