//
// Created by Mikhail Shkarubski on 13.11.23.
//
#include "tools.hpp"

void print(const FT::CImage& matrix, const bool real) {
    for (const auto& row : matrix) {
        for (const auto& element : row)
            std::cout << (real ? (double) element.real() : (double) element.imag()) << " ";

        std::cout << '\n';
    }
}

void MatToCImage(cv::Mat& src, FT::CImage& dst) {
    CV_Assert(!src.empty() && !dst.empty());
    CV_Assert(src.rows == dst.size() && src.cols == dst[0].size());

    src.convertTo(src, CV_64F);

    for (int i = 0; i < src.rows; ++i)
        for (int j = 0; j < src.cols; ++j)
            dst[i][j] = src.at<double>(i, j);
}

void CImageToMat(FT::CImage& src, cv::Mat& dst) {
    CV_Assert(!src.empty() && !dst.empty());
    CV_Assert(src.size() == dst.rows && src[0].size() == dst.cols);

    dst.convertTo(dst, CV_64F);

    for (int i = 0; i < src.size(); ++i)
        for (int j = 0; j < src[0].size(); ++j)
            dst.at<double>(i, j) = src[i][j].real();
}

FT::CImage gaussian(double mean, double std, int size) {
    FT::CImage gaussian(size, FT::CVector(size));

    const double exp_coef             = 1.0 / sqrt(2.0 * M_PI * std * std);
    const double exp_pd               = -1.0 / (2.0 * std * std);

    double sum = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double x = pow(j - mean, 2),
                   y = pow(i - mean, 2);

            gaussian[i][j] = FT::Complex(exp_coef * exp(exp_pd * (x + y)), 0);
            sum += gaussian[i][j].real();
        }
    }

    // Normalize the Gaussian matrix
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            gaussian[i][j] /= sum;

    return gaussian;
}