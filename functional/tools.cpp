//
// Created by Mikhail Shkarubski on 13.11.23.
//
#include "tools.hpp"

void print(const FT::DCImage& matrix, const int mode) {
    assert(mode == REAL_ONLY || mode == IMAG_ONLY || mode == REAL_IMAG);

    for (const auto& row : matrix) {
        for (const auto& element : row)
            std::cout << (mode == 0 ? element.real() : mode == 1 ? element.imag() : element) << " ";

        std::cout << '\n';
    }
}

void MatToCImage(cv::Mat& src, FT::DCImage& dst) {
    CV_Assert(!src.empty() && !dst.empty());
    CV_Assert(src.rows == dst.size() && src.cols == dst[0].size());

    src.convertTo(src, CV_64F);

    for (int i = 0; i < src.rows; ++i)
        for (int j = 0; j < src.cols; ++j)
            dst[i][j] = src.at<double>(i, j);
}

void CImageToMat(FT::DCImage& src, cv::Mat& dst) {
    CV_Assert(!src.empty() && !dst.empty());
    CV_Assert(src.size() == dst.rows && src[0].size() == dst.cols);

    dst.convertTo(dst, CV_64F);

    for (int i = 0; i < src.size(); ++i)
        for (int j = 0; j < src[0].size(); ++j)
            dst.at<double>(i, j) = src[i][j].real();
}

FT::DCImage gaussian(double mean, double std, int size) {
    FT::DCImage gaussian(size, FT::DCVector(size));

    const double exp_coef             = 1.0 / sqrt(2.0 * M_PI * std * std);
    const double exp_pd               = -1.0 / (2.0 * std * std);

    double sum = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            double x = pow(j - mean, 2),
                   y = pow(i - mean, 2);

            gaussian[i][j] = FT::DComplex(exp_coef * exp(exp_pd * (x + y)), 0);
            sum += gaussian[i][j].real();
        }
    }

    // Normalize the Gaussian matrix
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            gaussian[i][j] /= sum;

    return gaussian;
}