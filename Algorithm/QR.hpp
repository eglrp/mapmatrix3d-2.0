#ifndef MY_QR_HPP
#define MY_QR_HPP

#include <iostream>
#include <math.h>
#include "my_matrix_compu.hpp"

using namespace std;

extern double dot(double* a, double* b, int n);

extern void add(double* dst, double* src, int n);

extern double* kMultV(double cof, double* v, int n);

extern void getGama(int n, double* beta, double* gama);

extern void myQR(double* A, int n, double* Q, double* R);

#endif