//
// Created by Mikhail Shkarubski on 13.11.23.
//
#include "tools.h"

void print(CImage& matrix, bool real) {
    for (const auto& row : matrix) {
        for (const auto& element : row)
            std::cout << (real ? (double) element.real() : (double) element.imag()) << " ";

        std::cout << std::endl;
    }
}

void MatToCImage(cv::Mat& src, CImage& dst) {
    CV_Assert(!src.empty() && !dst.empty());
    CV_Assert(src.rows == dst.size() && src.cols == dst[0].size());

    src.convertTo(src, CV_64F);

    for (int i = 0; i < src.rows; ++i)
        for (int j = 0; j < src.cols; ++j)
            dst[i][j] = src.at<double>(i, j);
}

void CImageToMat(CImage& src, cv::Mat& dst) {
    CV_Assert(!src.empty() && !dst.empty());
    CV_Assert(src.size() == dst.rows && src[0].size() == dst.cols);

    dst.convertTo(dst, CV_64F);

    for (int i = 0; i < src.size(); ++i)
        for (int j = 0; j < src[0].size(); ++j)
            dst.at<double>(i, j) = src[i][j].real();
}