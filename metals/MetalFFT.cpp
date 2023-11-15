//
// Created by Mikhail Shkarubski on 14.11.23.

#include "MetalFFT.hpp"


/**
 * \brief Initializes MetalFFT class instance with MTL device to perform 1D FFT on and the size of the signal.
 * \param device MTL device that will handle the computations
 * \param arraySize size of signal to apply FFT to
 */
MetalFFT::MetalFFT(MTL::Device *device, uint arraySize): _mDevice(device), bufferSize(arraySize * sizeof(double)) {
    NS::Error *error = nullptr;
    MTL::Library *defaultLibrary = _mDevice->newDefaultLibrary();

    if (defaultLibrary == nullptr) {
        std::cout << "Failed to find the default library." << '\n';
        return;
    }

    const auto *str = NS::String::string("fft", NS::ASCIIStringEncoding);
    MTL::Function *function = defaultLibrary->newFunction(str);
    defaultLibrary->release();

    if (function == nullptr) {
        std::cout << "Failed to find the FFT function." << '\n';
        return;
    }

    // Create a compute pipeline state object.
    _mFunction = _mDevice->newComputePipelineState(function, &error);
    function->release();

    if (_mFunction == nullptr) {
        std::cout << "Failed to created pipeline state object." << '\n';
        return;
    }

    _mCommandQueue = _mDevice->newCommandQueue();
    if (_mCommandQueue == nullptr)
    {
        std::cout << "Failed to find the command queue." << '\n';
        return;
    }

    // Allocate three buffers to hold our initial data and the result.
    _mBufferX = _mDevice->newBuffer(bufferSize, MTL::ResourceStorageModeShared);
}

MetalFFT::~MetalFFT() {
    _mFunction->release();
    _mCommandQueue->release();
    _mBufferX->release();
}

void MetalFFT::executeFunction() {
    auto commandBuffer = _mCommandQueue->commandBuffer();
    assert(commandBuffer != nullptr);

    MTL::ComputeCommandEncoder *computeEncoder = commandBuffer->computeCommandEncoder();
    assert(computeEncoder != nullptr);

    encodeAddCommand(computeEncoder);

    // End the compute pass.
    computeEncoder->endEncoding();

    // Execute the command.
    commandBuffer->commit();

    // Normally, you want to do other work in your app while the GPU is running,
    // but in this example, the code simply blocks until the calculation is complete.
    commandBuffer->waitUntilCompleted();
}

void MetalFFT::encodeAddCommand(MTL::ComputeCommandEncoder* computeEncoder) {

    // Encode the pipeline state object and its parameters.
    computeEncoder->setComputePipelineState(_mFunction);
    computeEncoder->setBuffer(_mBufferX, 0, 0);

    MTL::Size gridSize = MTL::Size::Make(bufferSize / sizeof(double), 1, 1);

    // Calculate a threadgroup size.
    NS::UInteger threadGroupSize = _mFunction->maxTotalThreadsPerThreadgroup();

    if (threadGroupSize > bufferSize / sizeof(double))
        threadGroupSize = bufferSize / sizeof(double);

    MTL::Size threadgroupSize = MTL::Size::Make(threadGroupSize, 1, 1);

    // Encode the compute command.
    computeEncoder->dispatchThreads(gridSize, threadgroupSize);
}

void MetalFFT::loadData(FT::CVector &src, MTL::Buffer *dst) {
    auto *dataPtr = static_cast<FT::Complex*>(dst->contents());

    for (int i = 0; i < src.size(); ++i)
        dataPtr[i] = src[i];
}
