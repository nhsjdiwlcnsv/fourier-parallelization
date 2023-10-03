//
// Created by Mikhail Shkarubski on 3.10.23.
//

#include "Filter.h"

Filter::Filter(
        double* data,
        int w,  int h,
        int cc, int cr
        ): data(data), w(w), h(h), cc(cc), cr(cr) { }

Filter::Filter(
        int w, int h,
        int cc, int cr
        ): w(w), h(h), cc(cc), cr(cr) {
    data = new double[w * h];
}

Filter::Filter(const Filter &fltr): Filter(fltr.data, fltr.w, fltr.h, fltr.cc, fltr.cr) { }
