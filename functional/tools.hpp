//
// Created by Mikhail Shkarubski on 13.11.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_TOOLS_H
#define FOURIER_PARALLELIZATION_TOOLS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "types.hpp"

template<typename T>
void print(cv::Mat& matrix) {
    for (size_t i = 0; i < matrix.rows; ++i) {
        for (size_t j = 0; j < matrix.rows; ++j)
            std::cout << static_cast<int>(matrix.at<T>(i, j)) << " ";

        std::cout << '\n';
    }
}

void print(FT::CImage & matrix, bool real);
void CImageToMat(FT::CImage & src, cv::Mat& dst);
void MatToCImage(cv::Mat& src, FT::CImage & dst);
FT::CImage gaussian(double mean, double std, int size);

#endif //FOURIER_PARALLELIZATION_TOOLS_H
