//
// Created by Mikhail Shkarubski on 24.10.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_FFT_H
#define FOURIER_PARALLELIZATION_FFT_H

#include "tools.hpp"


void fft(FT::CArray& x, bool inverse);
void fftshift(FT::CArray& x);
void fft2d(FT::CImage& image, bool inverse);
void fftshift2d(FT::CImage& image);
cv::Mat pad(cv::Mat& input, int rows, int cols, int value);
cv::Mat roi(cv::Mat& src, int x, int y, int width, int height);
cv::Mat conv2d(cv::Mat& image, cv::Mat& kernel);
cv::Mat conv2dfft(cv::Mat& image, cv::Mat& kernel);

#endif //FOURIER_PARALLELIZATION_FFT_H
