//
//  fft.metal
//
//
//  Created by Mikhail Shkarubski on 14.11.23.
//

#include <metal_stdlib>
using namespace metal;

typedef float2 Complex;

kernel void fft(device Complex* x [[buffer(0)]],
                const int N,
                const bool inverse) {
    const int gid = int(get_global_id(0));

    if (gid >= N)
        return;

    const float THETA = ((inverse ? -2.0 : 2.0) * M_PI / N) * float(gid);

    Complex even = x[gid];
    Complex odd = x[gid + N / 2] * Complex(cos(THETA), sin(THETA));

    x[gid] = even + odd;
    x[gid + N / 2] = even - odd;
    
     if (inverse) {
         x[gid] /= 2.0;
         x[gid + N / 2] /= 2.0;
     }
}
