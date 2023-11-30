//
// Created by Mikhail Shkarubski on 13.11.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_TOOLS_H
#define FOURIER_PARALLELIZATION_TOOLS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "types.hpp"

#define REAL_ONLY 0
#define IMAG_ONLY 1
#define REAL_IMAG 2

template<typename T>
void print(cv::Mat& matrix) {
    for (size_t i = 0; i < matrix.rows; ++i) {
        for (size_t j = 0; j < matrix.rows; ++j)
            std::cout << static_cast<int>(matrix.at<T>(i, j)) << " ";

        std::cout << '\n';
    }
}

void print(FT::DCImage & matrix, int mode);
void DCImageToMat(FT::DCImage & src, cv::Mat& dst);
void MatToDCImage(cv::Mat& src, FT::DCImage & dst);
FT::DCVector gaussian(double mean, double std, int size);
FT::DCImage gaussian2d(double mean, double std, int size);


#endif //FOURIER_PARALLELIZATION_TOOLS_H
