// 2D Image Convolution
#include "fftparallel.h"
#include "fft.cpp"

int main() {
    std::vector<std::pair<int, int>> abliba;

    for (int i = 0; i < 50; i++) {
        std::vector<std::vector<int>> image = {{1,  2,  0,  4,  5},
                                               {6,  0,  8,  0,  10},
                                               {0,  12, 13, 14, 0},
                                               {16, 0,  18, 0,  20},
                                               {21, 22, 0,  24, 25}};
        std::vector<std::vector<int>> kernel = {{-1, 0, 1},
                                                {-2, 0, 2},
                                                {-1, 0, 1}};

        auto beg1 = std::chrono::high_resolution_clock::now();
        auto result1 = fft::Conv(image, kernel);
        auto end1 = std::chrono::high_resolution_clock::now();

        auto beg2 = std::chrono::high_resolution_clock::now();
        auto result2 = fft::dftConv(image, kernel);
        auto end2 = std::chrono::high_resolution_clock::now();

        abliba.push_back(std::pair<int, int>(
            (int) (std::chrono::duration_cast<std::chrono::microseconds>(end1 - beg1).count()),
            (int) (std::chrono::duration_cast<std::chrono::microseconds>(end2 - beg2).count())
        ));

        std::cout << "Default convolution duration: "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end1 - beg1).count() << std::endl;
        std::cout << "DFT convolution duration: "
                  << std::chrono::duration_cast<std::chrono::microseconds>(end2 - beg2).count() << std::endl;
        std::cout << std::endl;

        for (const auto &row: result1) {
            for (auto pixel: row)
                std::cout << (double) pixel << " ";

            std::cout << std::endl;
        }

        std::cout << std::endl;

        for (const auto &row: result2) {
            for (auto pixel: row)
                std::cout << std::real(pixel) << " ";

            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    for (auto ahuh: abliba)
        std::cout << ahuh.first << ", " << ahuh.second << std::endl;

    return 0;
}
