#ifndef MY_MATRIX_COMPU_HPP
#define MY_MATRIX_COMPU_HPP


#include <math.h>
#include <iostream>

using namespace std;

//#ifndef DETMAXCOL
//#define DETMAXCOL 128//注意栈的大小有限，如果递归深度大，这个值就要改小
//#endif


//矩阵数乘
template<class T>
void KfactorMatrix(T* M, int m, int n, T k, T* kM);

//矩阵加法
template<class T>
void AddMatrix(T* M1, T* M2, int m, int n, T* sumM);

//矩阵乘法
template<class T>
void MatrixMuti1D(T* A1, int m1, int n1, T* A2, int m2, int n2, T* A3);

//m1行n1列 X m2行n2列
template<class T>
void MatrixMuti2D(int** a, int m1, int n1, int** b, int m2, int n2, int** c);

//打印矩阵
template<class T>
void PrintMatrix(T* M, int m, int n);

//返回矩阵对应坐标为ij的行列式的余子式矩阵(非代数余子式)，只有方阵行列式才有余子式，n为方阵的行数（列数）
template<class T>
void MofDetofMatrix(T* M, int n, int i, int j, T* Mij);

//计算矩阵行列式的值，按第一列展开，n为矩阵行数（列数）
template<class T>
T DetValueofMatrix(int n, T* M);

//求矩阵的伴随矩阵
template<class T>
void AdjointMatrix(T* M, int n, T* AM);

//矩阵的转置
template<class T>
void TransposeMatrix(T* M, int m, int n, T* TM);

//矩阵的逆矩阵
template<class T>
void InverseMatrix(T* M, int n, T* IM);

//矩阵归一化
template<class T>
void NormMat(T* m, int r, int c);

#include "my_matrix_compu.template"
#endif