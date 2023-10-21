//
// Created by Mikhail Shkarubski on 20.10.23.
//

#ifndef FOURIER_ALEXNET_H
#define FOURIER_ALEXNET_H

#include <string>

class AlexNet {
    class Conv {
        private:
            int in_channels;
            int out_channels;
            int ks;
            int stride;
            int padding;
            bool bias;
            std::string padding_mode;

        public:
            Conv(
                    int in_channels,
                    int out_channels,
                    int ks,
                    int stride,
                    int padding,
                    bool bias,
                    std::string padding_mode
            );

            int getInChannels();
            int getOutChannels();
            int getKernelSize();
            int getStride();
            int getPadding();
            bool getBias();
            std::string getPaddingMode();
        };

    class MaxPool {
        private:
            int ks;
            int stride;
            int padding;

        public:
            MaxPool();

            int getKernelSize() const { return this->ks; }
            int getStride() const { return this->stride; }
            int getPadding() const { return this->padding; }
        };

        struct Linear {

        };
};

#endif //FOURIER_ALEXNET_H