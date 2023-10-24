cmake_minimum_required(VERSION 3.26)

project(fourier_parallelization)

set(CMAKE_CXX_STANDARD 23)

add_executable(fourier_parallelization
        main.cpp
        AlexNet.h
        AlexNet.cpp
        fft.h
        fft.cpp
        fftparallel.cpp
        fftparallel.h
)

target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/Metal)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/Foundation)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/QuartzCore)