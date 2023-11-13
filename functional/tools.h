//
// Created by Mikhail Shkarubski on 13.11.23.
//
#pragma once

#ifndef FOURIER_PARALLELIZATION_TOOLS_H
#define FOURIER_PARALLELIZATION_TOOLS_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <complex>
#include <valarray>
#include <vector>

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
typedef std::vector<Complex> CVector;
typedef std::vector<CVector> CImage;

template<typename T>
void print(cv::Mat& matrix) {
    for (size_t i = 0; i < matrix.rows; ++i) {
        for (size_t j = 0; j < matrix.rows; ++j)
            std::cout << (int) matrix.at<T>(i, j) << " ";

        std::cout << std::endl;
    }
}

void print(CImage& matrix, bool real);
void CImageToMat(CImage& src, cv::Mat& dst);
void MatToCImage(cv::Mat& src, CImage& dst);

#endif //FOURIER_PARALLELIZATION_TOOLS_H
