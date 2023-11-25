#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "src/fft.hpp"


int main() {
    cv::Mat image = cv::imread("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/examples/elgato.jpeg", cv::IMREAD_GRAYSCALE);
    cv::Mat imageCopy = image.clone();
    cv::Mat imageCopy1 = image.clone();
    cv::Mat imageCopy2 = image.clone();
    cv::Mat cvKernel(81, 81, CV_64F);

    FT::DCImage kernel = gaussian2d(0, 5, 81);

    DCImageToMat(kernel, cvKernel);

    // Calculate the duration
    auto start1 = std::chrono::high_resolution_clock::now();
    const cv::Mat convImage = SR::conv2d(image, cvKernel);
    auto end1 = std::chrono::high_resolution_clock::now();
    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result1.jpg", convImage);

    auto start2 = std::chrono::high_resolution_clock::now();
    const cv::Mat convImage2 = SR::conv2dfft(imageCopy, cvKernel);
    auto end2 = std::chrono::high_resolution_clock::now();
    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result2.jpg", convImage2);

    auto start3 = std::chrono::high_resolution_clock::now();
    const cv::Mat convImage3 = MT::conv2dfft(imageCopy1, cvKernel);
    auto end3 = std::chrono::high_resolution_clock::now();
    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result3.jpg", convImage3);

    auto start4 = std::chrono::high_resolution_clock::now();
    const cv::Mat convImage4 = OMP::conv2dfft(imageCopy2, cvKernel);
    auto end4 = std::chrono::high_resolution_clock::now();
    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result4.jpg", convImage4);

    auto duration1 = std::chrono::duration_cast<FT::TimeUnit>(end1 - start1);
    auto duration2 = std::chrono::duration_cast<FT::TimeUnit>(end2 - start2);
    auto duration3 = std::chrono::duration_cast<FT::TimeUnit>(end3 - start3);
    auto duration4 = std::chrono::duration_cast<FT::TimeUnit>(end4 - start4);

    std::cout << "Naive convolution took " << duration1.count() << "ms to execute." << '\n';
    std::cout << "FFT convolution (Serial) took " << duration2.count() << "ms to execute." << '\n';
    std::cout << "FFT convolution (Metal) took " << duration3.count() << "ms to execute." << '\n';
    std::cout << "FFT convolution (OpenMP) took " << duration4.count() << "ms to execute." << '\n';

    // auto const shader_executor = std::make_unique<FFTExecutor>(NS::String::string("fft", NS::ASCIIStringEncoding));
    //
    // FT::DCVector a = gaussian(10.0, 9.0, static_cast<int>(pow(2, 22)));
    // FT::DCVector b(a);
    // FT::DCVector c(a);
    //
    // const auto start11 = std::chrono::high_resolution_clock::now();
    // SR::fft(a, false);
    // std::cout << "Serial FFT computed" << '\n';
    // const auto end11 = std::chrono::high_resolution_clock::now();
    //
    // const auto start21 = std::chrono::high_resolution_clock::now();
    // MT::fft(b, false, shader_executor);
    // std::cout << "GPU accelerated FFT computed" << '\n';
    // const auto end21 = std::chrono::high_resolution_clock::now();
    //
    // const auto start31 = std::chrono::high_resolution_clock::now();
    // OMP::fft(c, false);
    // std::cout << "CPU accelerated FFT computed" << '\n';
    // const auto end31 = std::chrono::high_resolution_clock::now();
    //
    // const auto duration11 = std::chrono::duration_cast<FT::TimeUnit>(end11 - start11);
    // const auto duration21 = std::chrono::duration_cast<FT::TimeUnit>(end21 - start21);
    // const auto duration31 = std::chrono::duration_cast<FT::TimeUnit>(end31 - start31);
    //
    // std::cout << "SR::fft result: \n";
    // for (int i = 0; i < 15; i++)
    //     std::cout << a[i] << " ";
    //
    // std::cout << "\n\n";
    //
    // std::cout << "MT::fft result: \n";
    // for (int i = 0; i < 15; i++)
    //      std::cout << b[i] << " ";
    //
    // std::cout << "\n\n";
    //
    // std::cout << "OMP::fft result: \n";
    // for (int i = 0; i < 15; i++)
    //     std::cout << c[i] << " ";
    //
    // std::cout << "\n\n";
    //
    // std::cout << "Serial FFT took " << duration11.count() << "ms to execute." << std::endl;
    // std::cout << "GPU accelerated FFT took " << duration21.count() << "ms to execute." << std::endl;
    // std::cout << "CPU accelerated FFT took " << duration31.count() << "ms to execute." << std::endl;

    return 0;
}
