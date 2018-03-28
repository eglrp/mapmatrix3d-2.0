#ifndef CV_PLUS_HPP
#define CV_PLUS_HPP

#include <iostream>
#include <vector>
#include <highgui.h>
#include <cv.h>
#include <cxcore.h>

using namespace std;



void cvmPrint(CvMat* M);
//清零指定矩阵
void cvmZero(CvMat* M);
//给指定矩阵所有元素赋值val
void cvmSetVal(CvMat* M, double val);
//计算矩阵所有元素的平方和
double cvmNorm2(CvMat* M);

//拷贝函数，M1是源，M2是目标
void cvmVectorCopy(vector<CvMat* >& M1, vector<CvMat* >& M2);

//新建拷贝函数，M1是源，M2是目标，拷贝的时候初始化M2
void cvmVectorNewCopy(vector<CvMat* >& M1, vector<CvMat* >& M2);

void cvReleaseVectorMat(vector<CvMat* >& M);

#endif