cmake_minimum_required(VERSION 3.26)

project(fourier_parallelization)

set(CMAKE_CXX_STANDARD 23)

add_executable(fourier_parallelization main.cpp AlexNet.h AlexNet.cpp)
