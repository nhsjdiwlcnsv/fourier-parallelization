//
// Created by Mikhail Shkarubski on 19.11.23.
//

#include "ShaderExecutor.hpp"

FFTExecutor::FFTExecutor(const NS::String *shader_name) {
    device_ = MTL::CreateSystemDefaultDevice();

    MTL::Library *library         = device_->newLibrary(lib_path_, &error_);
    MTL::Function *function       = library->newFunction(shader_name);

    command_queue_ = device_->newCommandQueue();
    pipeline_state_ = device_->newComputePipelineState(function, &error_);

    library->release();
    function->release();

    x_buffer_ = nullptr;
    th_sz_buffer_ = nullptr;
    inv_buffer_ = nullptr;
    even_buffer_ = nullptr;
    odd_buffer_ = nullptr;
}

FFTExecutor::~FFTExecutor() {
    error_->release();
    device_->release();
    command_queue_->release();
    pipeline_state_->release();

    x_buffer_->release();
    th_sz_buffer_->release();
    inv_buffer_->release();
    even_buffer_->release();
    odd_buffer_->release();
}

void FFTExecutor::send_command(
        const FT::FComplex* x,
        const size_t n_threads,
        const size_t& thread_size,
        const bool& inv
    ) {
    auto *command_buffer = command_queue_->commandBuffer();
    auto *command_encoder = command_buffer->computeCommandEncoder();

    command_encoder->setComputePipelineState(pipeline_state_);

    const MTL::Size threads_per_grid               = MTL::Size::Make(n_threads, 1, 1);
    const NS::UInteger max_threads_per_group       = pipeline_state_->maxTotalThreadsPerThreadgroup();
    const MTL::Size threads_per_group              = MTL::Size::Make(max_threads_per_group, 1, 1);

    x_buffer_         = device_->newBuffer(x, thread_size * n_threads * sizeof(FT::FComplex), MTL::ResourceStorageModeShared);
    th_sz_buffer_     = device_->newBuffer(&thread_size, sizeof(size_t), MTL::ResourceStorageModeShared);
    inv_buffer_       = device_->newBuffer(&inv, sizeof(bool), MTL::ResourceStorageModeShared);
    even_buffer_      = device_->newBuffer(thread_size * n_threads * sizeof(FT::FComplex) / 2, MTL::ResourceStorageModeShared);
    odd_buffer_       = device_->newBuffer(thread_size * n_threads * sizeof(FT::FComplex) / 2, MTL::ResourceStorageModeShared);

    command_encoder->setBuffer(x_buffer_, 0, 0);
    command_encoder->setBuffer(th_sz_buffer_, 0, 1);
    command_encoder->setBuffer(inv_buffer_, 0, 2);
    command_encoder->setBuffer(even_buffer_, 0, 3);
    command_encoder->setBuffer(odd_buffer_, 0, 4);

    command_encoder->dispatchThreads(threads_per_grid, threads_per_group);
    command_encoder->endEncoding();

    command_buffer->commit();
    command_buffer->waitUntilCompleted();
}
