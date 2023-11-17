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
                uint k                        [[thread_position_in_grid]])
{
    const float THETA = (inv ? -2.0 : 2.0) * M_PI_F / float(th_size);
    const size_t shift = k * th_size;
    const size_t hshift = shift / 2;
    
    for (size_t i = 0; i < th_size / 2; ++i)
        even[hshift + i] = signal[shift + (i * 2)];
        
    for (size_t i = 0; i < th_size / 2; ++i)
        odd[hshift + i] = signal[shift + (i * 2) + 1];
    
    float2 t;
    for (size_t i = 0; i < th_size / 2; ++i) {
        size_t si = shift + i;
        size_t hsi = shift / 2 + i;
        float angle = THETA * i;
//        float2 s_even = float2(signal[shift + i * 2]);
//        float2 s_odd = float2(signal[shift + i * 2 + 1]);

        t = float2(cos(angle), sin(angle));
//        t = float2(t.x * s_odd.x - t.y * s_odd.y, t.x * s_odd.y + t.y * s_odd.x);
        t = float2(t.x * odd[hsi].x - t.y * odd[hsi].y, t.x * odd[hsi].y + t.y * odd[hsi].x);
        
//        signal[si] = float2(s_even + t);
//        signal[si + th_size / 2] = float2(s_even - t);
        signal[si] = float2(even[hsi] + t);
        signal[si + th_size / 2] = float2(even[hsi] - t);
        
        if (inv) {
            signal[si] /= 2;
            signal[si + th_size / 2] /= 2;
        }
    }
}
