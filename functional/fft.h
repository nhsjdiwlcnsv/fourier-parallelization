//
// Created by Mikhail Shkarubski on 24.10.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_FFT_H
#define FOURIER_PARALLELIZATION_FFT_H

#include "tools.h"

void fft(CArray& x, bool inverse);
void fftshift(CArray& x);
void fft2d(CImage& image, bool inverse);
void fftshift2d(CImage& image);
cv::Mat pad(cv::Mat& input, int rows, int cols, int value);
cv::Mat conv2d(cv::Mat& image, cv::Mat& kernel);
cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel);

#endif //FOURIER_PARALLELIZATION_FFT_H
