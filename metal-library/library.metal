//
//  library.metal
//  fourier-parallelization
//
//  Created by Mikhail Shkarubski on 17.11.23.
//

#include <metal_stdlib>
#include <metal_math>
using namespace metal;

kernel void fft(
                device float2* signal         [[buffer(0)]],
                constant size_t& th_size      [[buffer(1)]],
                constant bool& inv            [[buffer(2)]],
                device float2* even           [[buffer(3)]],
                device float2* odd            [[buffer(4)]],
                uint gid                      [[thread_position_in_grid]])
{
    const float THETA = (inv ? -2.0 : 2.0) * M_PI_F / float(th_size);
    const size_t shift = gid * th_size;
    const size_t hshift = shift / 2;

    for (size_t i = 0; i < th_size / 2; ++i)
        even[hshift + i] = signal[shift + (i * 2)];

    for (size_t i = 0; i < th_size / 2; ++i)
        odd[hshift + i] = signal[shift + (i * 2) + 1];

    float2 t;
    for (size_t i = 0; i < th_size / 2; ++i) {
        size_t si = shift + i;
        size_t hsi = hshift + i;
        float angle = THETA * i;

        t = float2(cos(angle), sin(angle));
        t = float2(t.x * odd[hsi].x - t.y * odd[hsi].y, t.x * odd[hsi].y + t.y * odd[hsi].x);

        signal[si] = float2(even[hsi] + t);
        signal[si + th_size / 2] = float2(even[hsi] - t);

        if (inv) {
            signal[si] /= 2;
            signal[si + th_size / 2] /= 2.0;
        }
    }
}


void fft1d(
                device float2* signal,
                device float2* odd,
                device float2* even,
                int size,
                constant bool& inverse) {

//    // Iterate through stages of FFT
//    for (int s = 1; s < size; s *= 2) {
//        int signalSize = s * 2;
//        float angle = (inverse ? -2.0 : 2.0) * M_PI_F / signalSize;
//
//        // Iterate through signal partitions
//        for (int offset = 0; offset < size; offset += signalSize) {
//
//            // Fill odd & even arrays
//            for (int k = 0; k < s; ++k) {
//                even[k] = float2(signal[offset + (k * 2)]);
//                odd[k] = float2(signal[offset + (k * 2) + 1]);
//            }
//
//            // Iterate through the current signal part
//            for (int k = 0; k < s; ++k) {
//                float2 t = float2(cos(angle * k), sin(angle * k));
//
//                t = float2(t.x * odd[k].x - t.y * odd[k].y, t.x * odd[k].y + t.y * odd[k].x);
//
//                signal[ k] = even[k] + t;
//                signal[offset + k + s] = even[k] - t;
//
//                // If performing inverse FFT, divide by size
//                if (inverse) {
//                    signal[offset + k] /= 2.0;
//                    signal[offset + k + s] /= 2.0;
//                }
//            }
//        }
//    }

//    if (size == 1)
//        return;
//
//    int halfSize = size / 2;
//    float theta = (inverse ? 2.0 : -2.0) * M_PI_F / size;
//
//    for (auto i = 0; i < halfSize; ++i) {
//        even[i] = signal[i * 2];
//        odd[i] = signal[i * 2 + 1];
//    }
//
//    fft1d(odd, &odd[halfSize], &even[halfSize], halfSize, inverse);
//    fft1d(even, &odd[halfSize], &even[halfSize], halfSize, inverse);
//
//    for (int k = 0; k < halfSize; ++k) {
//        float2 t = float2(cos(theta * k), sin(theta * k));
//        t = float2(t.x * odd[k].x - t.y * odd[k].y, t.x * odd[k].y + t.y * odd[k].x);
//
//        signal[k] = even[k] + t;
//        signal[k + halfSize] = even[k] - t;
//
//        if (inverse) {
//            signal[k] /= 2.0;
//            signal[k + halfSize] /= 2.0;
//        }
//    }

    for (int signalSize = 2; signalSize <= size; signalSize *= 2) {
        float theta = (inverse ? -2.0 : 2.0) * M_PI_F / signalSize;

        for (int i = 0; i < size; i += signalSize) {

            for (int k = 0; k < signalSize / 2; ++k) {
                even[i + k] = signal[i + k * 2];
                odd[i + k] = signal[i + k * 2 + 1];
            }

            for (int k = 0; k < signalSize / 2; ++k) {
                float2 t = float2(cos(theta * k), sin(theta * k));
                t = float2(t.x * odd[i + k].x - t.y * odd[i+k].y, t.x * odd[i+k].y + t.y * odd[i+k].x);

                signal[i + k] = even[i+k] + t;
                signal[i + k + signalSize / 2] = even[i+k] - t;

                if (inverse) {
                    signal[i + k] /= 2.0;
                    signal[i + k + signalSize / 2] /= 2.0;
                }
            }
        }
    }
}


kernel void fft2d(
                device float2* signal                  [[buffer(0)]],
                constant size_t& nRows                 [[buffer(1)]],
                constant size_t& nCols                 [[buffer(2)]],
                constant bool& inverse                 [[buffer(3)]],
                device float2* signalTransp            [[buffer(4)]],
                device float2* odd                     [[buffer(5)]],
                device float2* even                    [[buffer(6)]],
                uint gid                               [[thread_position_in_grid]]) {

    int rowShift = gid * nCols;
    int colShift = gid * nRows;

    // Perform 1D FFT along rows
    fft1d(&signal[rowShift], &odd[rowShift / 2], &even[rowShift / 2], nCols, inverse);

    threadgroup_barrier(mem_flags::mem_threadgroup);

    // Perform 1D FFT along columns
    // Copy column data to the buffer
    for (int i = 0; i < nRows; ++i)
        signalTransp[rowShift + i] = signal[gid + i * nRows];

    // Perform FFT on the column data
    fft1d(&signalTransp[colShift], &odd[colShift / 2], &even[colShift / 2], nRows, inverse);

    // Copy the result back to the original matrix
    for (int i = 0; i < nRows; ++i)
        signal[gid + i * nRows] = signalTransp[colShift + i];
}