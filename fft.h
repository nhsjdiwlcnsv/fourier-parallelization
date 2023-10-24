//
// Created by Mikhail Shkarubski on 24.10.23.
//

#ifndef FOURIER_PARALLELIZATION_FFT_H
#define FOURIER_PARALLELIZATION_FFT_H

#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <chrono>

#define freqMat std::vector<std::vector<std::complex<double>>>
#define freqVec std::vector<std::complex<double>>
#define freqMatT std::vector<std::vector<std::complex<T>>>
#define freqVecT std::vector<std::complex<T>>
#define cmplx std::complex<double>

namespace fft {
    template <class T, class U>
    std::vector<std::vector<T>> Conv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel);

    template <class T>
    std::vector<std::vector<T>> dftshift2d(std::vector<std::vector<T>> kernel, size_t rows, size_t cols);

    template <class T>
    freqMat dft2d(const std::vector<std::vector<T>>& image);

    template <class T>
    std::vector<std::vector<std::complex<T>>> idft2d(const freqMat& image);

    template <class T, class U>
    std::vector<std::vector<std::complex<T>>> dftConv(const std::vector<std::vector<T>>& image, const std::vector<std::vector<U>>& kernel);
}

#endif //FOURIER_PARALLELIZATION_FFT_H
