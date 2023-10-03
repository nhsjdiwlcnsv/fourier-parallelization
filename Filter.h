//
// Created by Mikhail Shkarubski on 3.10.23.
//
#include <iostream>

#ifndef FOURIER_FILTER_H
#define FOURIER_FILTER_H


struct Filter {
    double *data = nullptr;
    int w;
    int h;
    int cc;
    int cr;

    Filter(double* data, int w, int h, int cc, int cr);
    Filter(int w, int h, int cc, int cr);
    Filter(const Filter &filter);
};


#endif //FOURIER_FILTER_H
