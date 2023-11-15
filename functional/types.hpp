//
// Created by Mikhail Shkarubski on 14.11.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_TYPES_H
#define FOURIER_PARALLELIZATION_TYPES_H

#include <complex>
#include <valarray>
#include <vector>
#include <chrono>

namespace FT {
    using Complex = std::complex<double>;
    using CArray = std::valarray<Complex>;
    using CVector = std::vector<Complex>;
    using CImage = std::vector<CVector>;
    using TimeUnit = std::chrono::milliseconds;
}

#endif //FOURIER_PARALLELIZATION_TYPES_H
