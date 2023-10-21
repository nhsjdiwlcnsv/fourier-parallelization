cmake_minimum_required(VERSION 3.26)
project(Fourier)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        Fourier
        main.cpp
        stb_image.h
        stb_image_write.h
        Image.h
        Image.cpp
        Filter.cpp
        Filter.h
        Filter.h
        AlexNet.h
        AlexNet.cpp
)
