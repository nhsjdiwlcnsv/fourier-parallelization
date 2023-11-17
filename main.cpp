#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "functional/fft.hpp"

int main() {
    // cv::Mat image = cv::imread("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/examples/lenna.jpeg", cv::IMREAD_GRAYSCALE);
    // cv::Mat imageCopy = image.clone();
    // cv::Mat cvKernel(9, 9, CV_64F);

    // (ms)
    // 617 – 14711
    // 630 – 14916
    // 694 – 15578

    // FT::CImage kernel = gaussian(0, 5, 9);
    //
    // CImageToMat(kernel, cvKernel);

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
    // auto start_time_1 = std::chrono::high_resolution_clock::now();
    // const cv::Mat convImage = SR::conv2d(image, cvKernel);
    // auto end_time_1 = std::chrono::high_resolution_clock::now();
    // cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result1.jpg", convImage);
    //
    // auto start_time_2 = std::chrono::high_resolution_clock::now();
    // const cv::Mat convImage2 = SR::conv2dfft(imageCopy, cvKernel);
    // auto end_time_2 = std::chrono::high_resolution_clock::now();
    // cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result2.jpg", convImage2);
    //
    // auto duration_1 = std::chrono::duration_cast<FT::TimeUnit>(end_time_1 - start_time_1);
    // auto duration_2 = std::chrono::duration_cast<FT::TimeUnit>(end_time_2 - start_time_2);
    //
    // std::cout << "Naive convolution took " << duration_1.count() << "ms to execute." << std::endl;
    // std::cout << "FFT convolution took " << duration_2.count() << "ms to execute." << std::endl;

    FT::DCArray a = {FT::DComplex(255, 144), FT::DComplex(0, 19), FT::DComplex(27, 29), FT::DComplex(234, 92), FT::DComplex(123, 0.12), FT::DComplex(012, 153), FT::DComplex(200, 83.123), FT::DComplex(182.989, 46.567)};
    FT::DCVector b = {FT::DComplex(255, 144), FT::DComplex(0, 19), FT::DComplex(27, 29), FT::DComplex(234, 92), FT::DComplex(123, 0.12), FT::DComplex(012, 153), FT::DComplex(200, 83.123), FT::DComplex(182.989, 46.567)};

    auto start1 = std::chrono::high_resolution_clock::now();
    SR::fft(a, false);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto start2 = std::chrono::high_resolution_clock::now();
    MT::fft(b, false);
    auto end2 = std::chrono::high_resolution_clock::now();

    auto duration1 = std::chrono::duration_cast<FT::TimeUnit>(end1 - start1);
    auto duration2 = std::chrono::duration_cast<FT::TimeUnit>(end2 - start2);

    std::cout << "SR::fft result: \n";
    for (auto& val : a)
    std::cout << val << " ";

    std::cout << "\n\n";

    std::cout << "MT::fft result: \n";
    for (auto& val : b)
        std::cout << val << " ";

    std::cout << "\n\n";

    std::cout << "Serial FFT took " << duration1.count() << "ms to execute." << std::endl;
    std::cout << "GPU accelerated FFT took " << duration2.count() << "ms to execute." << std::endl;

    // SR::fft(a, true);
    // MT::fft(b, true);
    //
    // for (auto& val : a)
    //     std::cout << val << " ";
    //
    // std::cout << "\n\n";
    //
    // for (auto& val : b)
    //     std::cout << val << " ";

    // FT::DCVector c = {FT::DComplex(0, 0), FT::DComplex(1, 0), FT::DComplex(2, 0), FT::DComplex(3, 0), FT::DComplex(4, 0), FT::DComplex(5, 0), FT::DComplex(6, 0), FT::DComplex(7, 0)};
    //
    // MT::fft(c, false);
    //
    // for (auto& val : c)
    //     std::cout << val << " ";

    return 0;
}
