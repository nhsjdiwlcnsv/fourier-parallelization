//
// Created by Mikhail Shkarubski on 14.11.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_TYPES_H
#define FOURIER_PARALLELIZATION_TYPES_H

#include <complex>
#include <valarray>
#include <vector>

namespace FT {
    using DComplex = std::complex<double>;
    using DCArray = std::valarray<DComplex>;
    using DCVector = std::vector<DComplex>;

    using FComplex = std::complex<float32_t>;
    using FCArray = std::valarray<FComplex>;
    using FCVector = std::vector<FComplex>;

    using DCImage = std::vector<DCVector>;
    using FCImage = std::vector<FCVector>;

    using TimeUnit = std::chrono::milliseconds;
}

#endif //FOURIER_PARALLELIZATION_TYPES_H
