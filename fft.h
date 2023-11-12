//
// Created by Mikhail Shkarubski on 24.10.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_FFT_H
#define FOURIER_PARALLELIZATION_FFT_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <complex>
#include <valarray>
#include <vector>

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
typedef std::vector<Complex> CVector;
typedef std::vector<std::vector<Complex>> CImage;

void fft(CArray& x);
void fft2d(CImage& image);

//#define freqMat std::vector<std::vector<std::complex<double>>>
//#define freqVec std::vector<std::complex<double>>
//#define freqMatT std::vector<std::vector<std::complex<T>>>
//#define freqVecT std::vector<std::complex<T>>
//#define cmplx std::complex<double>
//
//#include <iostream>
//#include <vector>
//#include <cmath>
//#include <complex>
//#include <chrono>
//#include <opencv2/opencv.hpp>
//
//
//namespace fftools {
//    bool fft(freqVec& x, int sign);
//
//    template <class T, class U>
//    std::vector<std::vector<T>> Conv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel);
//
//    template <class T>
//    std::vector<std::vector<T>> dftshift2d(std::vector<std::vector<T>> kernel, size_t rows, size_t cols);
//
//    template <class T>
//    freqMat dft2d(const std::vector<std::vector<T>>& image);
//
//    template <class T>
//    std::vector<std::vector<std::complex<T>>> idft2d(const freqMat& image);
//
//    template <class T, class U>
//    std::vector<std::vector<std::complex<T>>> dftConv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel);
//}
//
#endif //FOURIER_PARALLELIZATION_FFT_H
