#include "stdafx.h"
#include "cv_plus.hpp"

void cvmPrint(CvMat* M)
{
	int row = M->rows;
	int col = M->cols;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			cout<<cvmGet(M, i, j);
			if (j < col - 1)
			{
				cout<<",";
			}
		}
		cout<<endl;
	}
	cout<<endl;
}

void cvmZero(CvMat* M)
{
#define SET_TYPE
	int row = M->rows;
	int col = M->cols;

	int type = CV_MAT_TYPE(M->type);
	if (type == CV_32FC1)
	{
#define SET_TYPE float
	} 
	else
	{
		assert(type == CV_64FC1);
#define SET_TYPE double
	}

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			cvmSet(M, i, j, SET_TYPE(0));
		}
	}
}
//未测试
void cvmSetVal(CvMat* M, double val)
{
#define SET_TYPE
	int row = M->rows;
	int col = M->cols;

	int type = CV_MAT_TYPE(M->type);
	if (type == CV_32FC1)
	{
#define SET_TYPE float
	} 
	else
	{
		assert(type == CV_64FC1);
#define SET_TYPE double
	}

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			cvmSet(M, i, j, SET_TYPE(val));
		}
	}
}
//未测试
double cvmNorm2(CvMat* M)
{
	assert(NULL != M);
	int row = M->rows;
	int col = M->cols;
	double sum = 0.0;
	double vij;
	for (int i = 0; i < row; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			vij = cvmGet(M, i, j);
			sum += vij * vij;
		}
	}
	return sum;
}

//拷贝函数，M1是源，M2是目标
void cvmVectorCopy(vector<CvMat* >& M1, vector<CvMat* >& M2)
{
	int n = M1.size();
	assert(n != 0 && n == M2.size());
	for (int i = 0; i < n; ++i)
	{
		assert(M1[i]->rows == M2[i]->rows && M1[i]->cols == M2[i]->cols);
		for (int r = 0;  r < M1[i]->rows; ++r)
		{
			for (int c = 0; c < M1[i]->cols; ++c)
			{
				cvmSet(M2[i], r, c, cvmGet(M1[i], r, c));
			}
		}
	}
}
//新建拷贝函数，M1是源，M2是目标，拷贝的时候初始化M2
void cvmVectorNewCopy(vector<CvMat* >& M1, vector<CvMat* >& M2)
{
	int n = M1.size();
	assert(n != 0);
	for (int i = 0; i < n; ++i)
	{
		CvMat* o_i = cvCreateMat(M1[i]->rows, M1[i]->cols, CV_64FC1);
		for (int r = 0;  r < M1[i]->rows; ++r)
		{
			for (int c = 0; c < M1[i]->cols; ++c)
			{
				cvmSet(o_i, r, c, cvmGet(M1[i], r, c));
			}
		}
		M2.push_back(o_i);
	}
}

void cvReleaseVectorMat(vector<CvMat* >& M)
{
	int n = M.size();
	for (int  i = 0; i < n; ++i)
	{
		if (M[i] != NULL)
		{
			cvReleaseMat(&M[i]);
		}
	}
	M.clear();
}