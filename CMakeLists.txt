cmake_minimum_required(VERSION 3.26)

project(fourier_parallelization)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
        main.cpp
        AlexNet.cpp
        AlexNet.h
        fft.cpp
        fft.h
        fftparallel.cpp
        fftparallel.h
)

target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/Metal)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/Foundation)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/QuartzCore)
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})