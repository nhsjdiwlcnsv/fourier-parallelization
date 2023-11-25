//
// Created by Mikhail Shkarubski on 19.11.23.
//
#pragma once

#include "tools.hpp"
#include <Metal/Metal.hpp>
#include <Foundation/Foundation.hpp>
#include <QuartzCore/QuartzCore.hpp>

#ifndef SHADEREXECUTOR_H
#define SHADEREXECUTOR_H

class ShaderExecutor {
protected:
    MTL::Device               *device_                       = nullptr;
    NS::Error                 *error_                        = nullptr;
    MTL::CommandQueue         *command_queue_                = nullptr;
    MTL::ComputePipelineState *pipeline_state_               = nullptr;

    NS::String *lib_path_ = NS::String::string("/Users/mishashkarubski/CLionProjects/fourier-parallelization/metal-library/library.metallib", NS::ASCIIStringEncoding);
};

class FFTExecutor : ShaderExecutor {
public:
    explicit FFTExecutor(const NS::String *shader_name);
    ~FFTExecutor();

    void send_command(const FT::FComplex* x, size_t n_threads, const size_t& thread_size, const bool& inv);

    template <typename T>
    const T* exposeXBuffer();
    template <typename T>
    const T* exposeThSzBuffer();
    template <typename T>
    const T* exposeInvBuffer();

private:
    MTL::Buffer *x_buffer_;
    MTL::Buffer *th_sz_buffer_;
    MTL::Buffer *inv_buffer_;
    MTL::Buffer *even_buffer_;
    MTL::Buffer *odd_buffer_;
};

template<typename T>
const T* FFTExecutor::exposeXBuffer() {
    return static_cast<T*>(x_buffer_->contents());
}

template<typename T>
const T* FFTExecutor::exposeThSzBuffer() {
    return static_cast<T*>(th_sz_buffer_->contents());
}

template<typename T>
const T* FFTExecutor::exposeInvBuffer() {
    return static_cast<T*>(inv_buffer_->contents());
}


#endif //SHADEREXECUTOR_H
