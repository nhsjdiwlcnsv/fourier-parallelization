cmake_minimum_required(VERSION 3.26)

project(fourier-parallelization)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
        main.cpp
        nn/AlexNet.cpp
        nn/AlexNet.h
        functional/fft.cpp
        functional/fft.hpp
        functional/fftparallel.cpp
        functional/fftparallel.hpp
        functional/tools.cpp
        functional/tools.hpp
        metals/MetalFFT.cpp
        metals/MetalFFT.hpp
        functional/types.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/Metal)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/Foundation)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/QuartzCore)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

target_link_libraries(fourier-parallelization
        "-framework Metal"
        "-framework Foundation"
        "-framework QuartzCore"
)