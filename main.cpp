#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "functional/fftparallel.hpp"
#include "functional/fft.hpp"

int main() {
//    cv::Mat image(5, 5, CV_64F);
//    CImage cImage = {{1, 2, 0, 4, 5},
//                     {6, 0, 8, 0, 10},
//                     {0, 12, 13, 14, 0},
//                     {16, 0, 18, 0, 20},
//                     {21, 22, 0, 24, 25}};
//
//    CImageToMat(cImage, image);
    cv::Mat image = cv::imread("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/examples/lenna.jpeg", cv::IMREAD_GRAYSCALE);
    cv::Mat imageCopy = image.clone();
    cv::Mat cvKernel(9, 9, CV_64F);

    // (ms)
    // 617 – 14711
    // 630 – 14916
    // 694 – 15578

    FT::CImage kernel = gaussian(0, 5, 9);

    CImageToMat(kernel, cvKernel);

    // cv::Mat padImageCopy = pad(imageCopy, 2048, 2048, 0);
    // FT::CImage cImage(padImageCopy.rows, FT::CVector(padImageCopy.cols));
    // MatToCImage(padImageCopy, cImage);
    //
    // fft2d(cImage, false);
    // fftshift2d(cImage);
    // fft2d(cImage, true);
    // fftshift2d(cImage);
    //
    // CImageToMat(cImage, padImageCopy);
    //
    // padImageCopy = roi(padImageCopy, 424, 424, 1200, 1200);
    // imageCopy.convertTo(padImageCopy, CV_8U);
    // cv::normalize(imageCopy, padImageCopy, 0, 255, cv::NORM_MINMAX);
    //
    // cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result3.jpg", padImageCopy);

    // Calculate the duration
    auto start_time_1 = std::chrono::high_resolution_clock::now();
    const cv::Mat convImage = conv2d(image, cvKernel);
    auto end_time_1 = std::chrono::high_resolution_clock::now();
    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result1.jpg", convImage);

    auto start_time_2 = std::chrono::high_resolution_clock::now();
    const cv::Mat convImage2 = conv2dfft(imageCopy, cvKernel);
    auto end_time_2 = std::chrono::high_resolution_clock::now();
    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result2.jpg", convImage2);

    auto duration_1 = std::chrono::duration_cast<FT::TimeUnit>(end_time_1 - start_time_1);
    auto duration_2 = std::chrono::duration_cast<FT::TimeUnit>(end_time_2 - start_time_2);

    std::cout << "Naive convolution took " << duration_1.count() << "ms to execute." << std::endl;
    std::cout << "FFT convolution took " << duration_2.count() << "ms to execute." << std::endl;

    return 0;
}
