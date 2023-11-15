//
// Created by Mikhail Shkarubski on 24.10.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_FFTPARALLEL_H
#define FOURIER_PARALLELIZATION_FFTPARALLEL_H

#include "../metals/MetalFFT.hpp"

namespace FTP {
    void fft(FT::CVector& x, bool inverse);
}

#endif //FOURIER_PARALLELIZATION_FFTPARALLEL_H
