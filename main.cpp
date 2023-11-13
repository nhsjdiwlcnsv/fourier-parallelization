#include "functional/fftparallel.h"
#include "functional/fft.h"

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
    cv::Mat cvKernel(3, 3, CV_64F);

    CImage kernel = {{-1, 0, 1},
                     {-2, 0, 2},
                     {-1, 0, 1}};

    CImageToMat(kernel, cvKernel);

//    cv::Mat padImageCopy = pad(imageCopy, 2048, 2048, 0);
    CImage cImage(imageCopy.rows, CVector(imageCopy.cols));
    MatToCImage(imageCopy, cImage);

    fft2d(cImage, false);
    fftshift2d(cImage);
    fft2d(cImage, true);
    fftshift2d(cImage);

    CImageToMat(cImage, imageCopy);
    imageCopy.convertTo(imageCopy, CV_8U);
    cv::normalize(imageCopy, imageCopy, 0, 255, cv::NORM_MINMAX);

    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result3.jpg", imageCopy);

    cv::Mat convImage = conv2d(image, cvKernel);
    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result1.jpg", convImage);

    cv::Mat convImage2 = conv2dfft(imageCopy, cvKernel);
    cv::imwrite("/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/result2.jpg", convImage2);

    return 0;
}
