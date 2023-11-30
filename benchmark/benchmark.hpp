//
// Created by Mikhail Shkarubski on 30.11.23.
//

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "../src/fft.hpp"

#define BENCHMARK_FFT           0
#define BENCHMARK_FFT2D         1
#define BENCHMARK_CONV          2

#define KERNEL_SIZE             81

using hires_clock = std::chrono::high_resolution_clock;
using timepoint = std::chrono::time_point<std::chrono::steady_clock>;


inline void benchmark(int start, int end, int mode, const std::string& name) {
    std::string path = "/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/data/";
    std::string filename = name + ".csv";
    std::fstream df;

    df.open(path + filename, std::fstream::out);

    if (mode == BENCHMARK_FFT || mode == BENCHMARK_FFT2D)
        df << "size,serial,metal,openmp" << '\n';
    else if (mode == BENCHMARK_CONV)
        df << "image_size,kernel_size,serial,metal,openmp" << '\n';

    const auto shader_executor = std::make_unique<FFTExecutor>(NS::String::string("fft", NS::ASCIIStringEncoding));

    for (int i = start; i <= end; i *= 2) {
        if (mode == BENCHMARK_FFT) {
            FT::DCVector x = gaussian(0, 25, i);
            FT::DCVector sr_x(x), mt_x(x), omp_x(x);

            timepoint start1 = hires_clock::now();
            SR::fft(sr_x, false);
            timepoint stop1 = hires_clock::now();

            timepoint start2 = hires_clock::now();
            MT::fft(mt_x, false, shader_executor);
            timepoint stop2 = std::chrono::high_resolution_clock::now();

            timepoint start3 = hires_clock::now();
            OMP::fft(omp_x, false);
            timepoint stop3 = hires_clock::now();

            auto duration1 = std::chrono::duration_cast<FT::TimeUnit>(stop1 - start1).count();
            auto duration2 = std::chrono::duration_cast<FT::TimeUnit>(stop2 - start2).count();
            auto duration3 = std::chrono::duration_cast<FT::TimeUnit>(stop3 - start3).count();

            df << i << "," << duration1 << "," << duration2 << "," << duration3 << "\n";
        }
        else if (mode == BENCHMARK_FFT2D) {
            FT::DCImage x = gaussian2d(0, 25, i);
            FT::DCImage sr_x(x), mt_x(x), omp_x(x);

            timepoint start1 = hires_clock::now();
            SR::fft2d(sr_x, false);
            timepoint stop1 = hires_clock::now();

            timepoint start2 = hires_clock::now();
            MT::fft2d(mt_x, false);
            timepoint stop2 = std::chrono::high_resolution_clock::now();

            timepoint start3 = hires_clock::now();
            OMP::fft2d(omp_x, false);
            timepoint stop3 = hires_clock::now();

            auto duration1 = std::chrono::duration_cast<FT::TimeUnit>(stop1 - start1).count();
            auto duration2 = std::chrono::duration_cast<FT::TimeUnit>(stop2 - start2).count();
            auto duration3 = std::chrono::duration_cast<FT::TimeUnit>(stop3 - start3).count();

            df << i << "," << duration1 << "," << duration2 << "," << duration3 << "\n";
        }
        else if (mode == BENCHMARK_CONV) {
            FT::DCImage kernel = gaussian2d(0, 25, KERNEL_SIZE);
            FT::DCImage image = gaussian2d(100, 100, i);

            cv::Mat cvImage(image.size(), image[0].size(), CV_64F);
            cv::Mat cvKernel(kernel.size(), kernel[0].size(), CV_64F);

            DCImageToMat(kernel, cvKernel);
            DCImageToMat(image, cvImage);

            cv::Mat sr_x        = cvImage.clone(),
                    mt_x        = cvImage.clone(),
                    omp_x       = cvImage.clone(),
                    naive_x     = cvImage.clone();

            timepoint start0 = hires_clock::now();
            SR::conv2d(naive_x, cvKernel);
            timepoint stop0 = hires_clock::now();

            timepoint start1 = hires_clock::now();
            SR::conv2dfft(sr_x, cvKernel);
            timepoint stop1 = hires_clock::now();

            timepoint start2 = hires_clock::now();
            MT::conv2dfft(mt_x, cvKernel);
            timepoint stop2 = hires_clock::now();

            timepoint start3 = hires_clock::now();
            OMP::conv2dfft(omp_x, cvKernel);
            timepoint stop3 = hires_clock::now();

            auto duration0 = std::chrono::duration_cast<FT::TimeUnit>(stop0 - start0).count();
            auto duration1 = std::chrono::duration_cast<FT::TimeUnit>(stop1 - start1).count();
            auto duration2 = std::chrono::duration_cast<FT::TimeUnit>(stop2 - start2).count();
            auto duration3 = std::chrono::duration_cast<FT::TimeUnit>(stop3 - start3).count();

            df << i << "," << KERNEL_SIZE << "," << duration0 << "," << duration1 << "," << duration2 << "," << duration3 << "\n";
        }

        std::cout << "Passed iteration (mode " << mode << ", size " << i << ')' << '\n';
    }

    std::cout << "Finished benchmarking " << (mode == 0 ? "1D FFT." : mode == 1 ? "2D FFT." : "CONVOLUTON.") << "\n\n";

    df.close();
}

#endif //BENCHMARK_H
