//
// Created by Mikhail Shkarubski on 24.10.23.
//

#include "fftparallel.hpp"

void FTP::fft(FT::CVector& x, bool inverse) {
    const int N = x.size();
    const double THETA = (inverse ? 2.0 : -2.0) * M_PI / N;

    if (N <= 1)
        return;

    auto *device = MTL::CreateSystemDefaultDevice();
    auto *metalFFT = new MetalFFT(device, N * sizeof(FT::Complex));

    metalFFT->loadData(x, metalFFT->_mBufferX);
    metalFFT->executeFunction();
}