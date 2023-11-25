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
    // Base complex type
    using DComplex = std::complex<double>;
    using FComplex = std::complex<float32_t>;

    // Complex vactor type
    using DCVector = std::vector<DComplex>;
    using FCVector = std::vector<FComplex>;

    // Complex image type
    using DCImage = std::vector<DCVector>;
    using FCImage = std::vector<FCVector>;

    // Base time unit type
    using TimeUnit = std::chrono::milliseconds;
}

#endif //FOURIER_PARALLELIZATION_TYPES_H
