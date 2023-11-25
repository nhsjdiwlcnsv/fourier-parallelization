#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION

#include "src/fft.hpp"


int main() {
    // cv::Mat image = cv::imread("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/examples/voices.jpeg", cv::IMREAD_GRAYSCALE);
    // cv::Mat imageCopy = image.clone();
    // cv::Mat imageCopy1 = image.clone();
    // cv::Mat cvKernel(81, 81, CV_64F);
    //
    // FT::DCImage kernel = gaussian2d(0, 25, 81);
    //
    // DCImageToMat(kernel, cvKernel);
    //
    // cv::Mat padImageCopy = pad(imageCopy, 512, 512, 0);
    // FT::DCImage cImage(padImageCopy.rows, FT::DCVector(padImageCopy.cols));
    // MatToDCImage(padImageCopy, cImage);
    //
    // MT::fft2d(cImage, false);
    // // SR::fftshift2d(cImage);
    // MT::fft2d(cImage, true);
    // // SR::fftshift2d(cImage);
    //
    // DCImageToMat(cImage, padImageCopy);
    //
    // padImageCopy = roi(padImageCopy, 7, 7, 498, 498);
    // padImageCopy.convertTo(padImageCopy, CV_8U);
    // cv::normalize(padImageCopy, padImageCopy, 0, 255, cv::NORM_MINMAX);
    //
    // cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result0.jpg", padImageCopy);
    //
    // // Calculate the duration
    // auto start1 = std::chrono::high_resolution_clock::now();
    // const cv::Mat convImage = SR::conv2d(image, cvKernel);
    // auto end1 = std::chrono::high_resolution_clock::now();
    // cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result1.jpg", convImage);
    //
    // auto start2 = std::chrono::high_resolution_clock::now();
    // const cv::Mat convImage2 = MT::conv2dfft(imageCopy, cvKernel);
    // auto end2 = std::chrono::high_resolution_clock::now();
    // cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result2.jpg", convImage2);
    //
    // auto start3 = std::chrono::high_resolution_clock::now();
    // const cv::Mat convImage3 = SR::conv2dfft(imageCopy1, cvKernel);
    // auto end3 = std::chrono::high_resolution_clock::now();
    // cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result3.jpg", convImage3);
    //
    // auto duration1 = std::chrono::duration_cast<FT::TimeUnit>(end1 - start1);
    // auto duration2 = std::chrono::duration_cast<FT::TimeUnit>(end2 - start2);
    // auto duration3 = std::chrono::duration_cast<FT::TimeUnit>(end3 - start3);
    //
    // std::cout << "Naive convolution (SR) took " << duration1.count() << "ms to execute." << std::endl;
    // std::cout << "FFT convolution (MT) took " << duration2.count() << "ms to execute." << std::endl;
    // std::cout << "FFT convolution (SR) took " << duration3.count() << "ms to execute." << std::endl;

    auto const shader_executor = std::make_unique<FFTExecutor>(NS::String::string("fft", NS::ASCIIStringEncoding));

    FT::DCVector a = gaussian(10.0, 9.0, static_cast<int>(pow(2, 22)));
    FT::DCVector b(a);
    FT::DCVector c(a);

    const auto start11 = std::chrono::high_resolution_clock::now();
    SR::fft(a, false);
    std::cout << "Serial FFT computed" << '\n';
    const auto end11 = std::chrono::high_resolution_clock::now();

    const auto start21 = std::chrono::high_resolution_clock::now();
    MT::fft(b, false, shader_executor);
    std::cout << "GPU accelerated FFT computed" << '\n';
    const auto end21 = std::chrono::high_resolution_clock::now();

    const auto start31 = std::chrono::high_resolution_clock::now();
    OMP::fft(c, false);
    std::cout << "CPU accelerated FFT computed" << '\n';
    const auto end31 = std::chrono::high_resolution_clock::now();

    const auto duration11 = std::chrono::duration_cast<FT::TimeUnit>(end11 - start11);
    const auto duration21 = std::chrono::duration_cast<FT::TimeUnit>(end21 - start21);
    const auto duration31 = std::chrono::duration_cast<FT::TimeUnit>(end31 - start31);

    std::cout << "SR::fft result: \n";
    for (int i = 0; i < 15; i++)
        std::cout << a[i] << " ";

    std::cout << "\n\n";

    std::cout << "MT::fft result: \n";
    for (int i = 0; i < 15; i++)
         std::cout << b[i] << " ";

    std::cout << "\n\n";

    std::cout << "OMP::fft result: \n";
    for (int i = 0; i < 15; i++)
        std::cout << c[i] << " ";

    std::cout << "\n\n";

    std::cout << "Serial FFT took " << duration11.count() << "ms to execute." << std::endl;
    std::cout << "GPU accelerated FFT took " << duration21.count() << "ms to execute." << std::endl;
    std::cout << "CPU accelerated FFT took " << duration31.count() << "ms to execute." << std::endl;

    // ====================== OpenMP practice ==========================

    // static long num_steps = 1000000, n_threads = omp_get_max_threads();
    // double pi = 0.0, step = 1.0 / static_cast<double>(num_steps);
    // int i;
    //
    // omp_set_num_threads(n_threads);
    //
    // double start = omp_get_wtime();
    //
    // #pragma omp parallel
    // {
    //     double x, sum = 0.0;
    //     const int pid = omp_get_thread_num(),
    //               nthrds = omp_get_num_threads();
    //
    //     if (pid == MASTER_THREAD)
    //         n_threads = nthrds;
    //
    //     for (int i = pid; i < num_steps; i += nthrds) {
    //         x = (i + 0.5) * step;
    //         sum += 4.0 / (1.0 + x * x);
    //     }
    //
    //     #pragma omp critical
    //         for (i = 0; i < n_threads; i++) pi += sum * step;
    // }
    //
    // double end = omp_get_wtime();
    //
    // std::cout << pi << '\n';
    // std::cout << end - start << '\n';
    // std::cout << n_threads << '\n';

    return 0;
}
