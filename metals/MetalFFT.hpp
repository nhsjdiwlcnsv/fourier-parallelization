//
// Created by Mikhail Shkarubski on 14.11.23.
#pragma once

#ifndef FOURIER_PARALLELIZATION_METALFFT_H
#define FOURIER_PARALLELIZATION_METALFFT_H

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "../functional/tools.hpp"

/**
 * \brief Class for handling parallel GPU computation of one-dimensional Fast Fourier Transform (FFT)
 */
class MetalFFT {
public:
    MTL::Device *_mDevice;
    MTL::ComputePipelineState *_mFunction;
    MTL::CommandQueue *_mCommandQueue;
    MTL::Buffer *_mBufferX;

    explicit MetalFFT(MTL::Device *device, uint arraySize);
    ~MetalFFT();

    void loadData(FT::CVector &src, MTL::Buffer *dst);
    void executeFunction();

private:
    uint bufferSize;
    void encodeAddCommand(MTL::ComputeCommandEncoder *computeEncoder);
};


#endif //FOURIER_PARALLELIZATION_METALFFT_H
