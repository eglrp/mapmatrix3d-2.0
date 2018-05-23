#include "StdAfx.h"
#include "SevenParameter.h"
#include "cv_plus.hpp"
#include "my_matrix_compu.hpp"
#include "projection_matrix_transform.hpp"
#include "qcomm.h"
#include "InterfaceOsgAlgorithm.h"

CSevenParameter::CSevenParameter(void)
{
	seven_parm_is_exist_ = false;
	X_a_.clear();
	X_r_.clear();
}


CSevenParameter::~CSevenParameter(void)
{
	seven_parm_is_exist_ = false;
	X_a_.clear();
	X_r_.clear();
}


void CSevenParameter::LoadControlPoints(vector<CoordDouble3D>& X_a, vector<CoordDouble3D>& X_r)
{
	if (X_a.size() == 0 || X_r.size() == 0)
	{
		cout<<"Failed to load control points !"<<endl;
		exit(-1);
	}
	X_a_ = X_a;
	X_r_ = X_r;
}

//计算绝对定向七参数，绝对定向坐标X_a，相对定向坐标X_r
void CSevenParameter::CalculateSevenParameter()
{
	if (X_a_.size() == 0 || X_r_.size() == 0 || X_a_.size() != X_r_.size())
	{
		cout<<"points numbers error!"<<endl;
		exit(-1);
	}
	int num_points = X_a_.size();

	double bar_X_a[3] = {0.0, 0.0, 0.0};
	double bar_X_r[3] = {0.0, 0.0, 0.0};
	//先归一化
	for (int i = 0; i < num_points; ++i)
	{
		bar_X_a[0] += X_a_[i].x;
		bar_X_a[1] += X_a_[i].y;
		bar_X_a[2] += X_a_[i].z;
		bar_X_r[0] += X_r_[i].x;
		bar_X_r[1] += X_r_[i].y;
		bar_X_r[2] += X_r_[i].z;
	}

	bar_X_a[0] /= num_points;
	bar_X_a[1] /= num_points;
	bar_X_a[2] /= num_points;
	bar_X_r[0] /= num_points;
	bar_X_r[1] /= num_points;
	bar_X_r[2] /= num_points;

	vector<CoordDouble3D> X_a1;
	vector<CoordDouble3D> X_r1;

	for (int i = 0; i < num_points; ++i)
	{
		X_a1.push_back(CoordDouble3D(X_a_[i].x - bar_X_a[0], X_a_[i].y - bar_X_a[1], X_a_[i].z - bar_X_a[2]));
		X_r1.push_back(CoordDouble3D(X_r_[i].x - bar_X_r[0], X_r_[i].y - bar_X_r[1], X_r_[i].z - bar_X_r[2]));
	}

	double Sxx = 0.0;
	double Syy = 0.0;
	double Szz = 0.0;
	double Sxy = 0.0;
	double Syx = 0.0;
	double Syz = 0.0;
	double Szy = 0.0;
	double Sxz = 0.0;
	double Szx = 0.0;
	
	for (int i = 0; i < num_points; ++i)
	{
		Sxx += X_r1[i].x * X_a1[i].x;
		Syy += X_r1[i].y * X_a1[i].y;
		Szz += X_r1[i].z * X_a1[i].z;
		Sxy += X_r1[i].x * X_a1[i].y;
		Syx += X_r1[i].y * X_a1[i].x;
		Syz += X_r1[i].y * X_a1[i].z;
		Szy += X_r1[i].z * X_a1[i].y;
		Sxz += X_r1[i].x * X_a1[i].z;
		Szx += X_r1[i].z * X_a1[i].x;
	}

	double N[16];
	N[0] = Sxx + Syy + Szz; N[1] = Syz - Szy; N[2] = Szx - Sxz; N[3] = Sxy - Syx;
	N[4] = Syz - Szy; N[5] = Sxx - Syy - Szz; N[6] = Sxy + Syx; N[7] = Szx + Sxz;
	N[8] = Szx - Sxz; N[9] = Sxy + Syx; N[10] = -Sxx + Syy - Szz; N[11] = Syz + Szy;
	N[12] = Sxy - Syx; N[13] = Szx + Sxz; N[14] = Syz + Szy; N[15] = -Sxx - Syy + Szz;

	//计算特征向量
	CvMat* cvN = cvCreateMat(4, 4, CV_64FC1);
	CvMat* cvEV = cvCreateMat(4, 4, CV_64FC1);
	CvMat* cvE = cvCreateMat(4, 1, CV_64FC1);
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cvmSet(cvN, i, j, N[i * 4 + j]);
		}
	}

	cvEigenVV(cvN, cvEV, cvE); //opencv计算的特征值从大到小排列
	//矩阵要转置，opencv的特征向量是行矢量
	//求最大特征值对应的特征向量
	if (cvmGet(cvE, 0, 0) < 0.0)
	{
		cout<<"Eigen value can not be negative!"<<endl;
		exit(-1);
	}

	double EV[4];
	
	for (int j = 0; j < 4; ++j)
	{
		EV[j] = cvmGet(cvEV, 0, j);
	}


	InterfaceOsgAlgorithmFactory IAlgorithmFactory;
	InterfaceOsgAlgorithm* IAlgorithm = IAlgorithmFactory.create();
	IAlgorithm->CvmPrint(cvE);

	double q0, qx, qy, qz;
	q0 = EV[0];
	qx = EV[1];
	qy = EV[2];
	qz = EV[3];
	//用“Closed-form solution of absolute orientation using unit quaternions”里的公式是错的
	//R[0] = q0 * q0 + qx * qx - qy * qy - qz * qz;
	//R[1] = 2.0 * (qx * qy - q0 * qz);
	//R[2] = 2.0 * (qx * qz + q0 * qy);
	//R[3] = 2.0 * (qy * qx + q0 * qz);
	//R[4] = q0 * q0 - qx * qx + qy * qy - qz * qz;
	//R[5] = 2.0 * (qy * qz - q0 * qx);
	//R[6] = 2.0 * (qz * qx - q0 * qy);
	//R[7] = 2.0 * (qz * qy + q0 * qz);
	//R[8] = q0 * q0 - qx * qx - qy * qy + qz * qz;

	//用“四元數與旋轉”里的公式是对的
	R_[0] = 1.0 - 2.0 * (qy * qy + qz * qz);
	R_[1] = 2.0 * (qx * qy - q0 * qz);
	R_[2] = 2.0 * (qx * qz + q0 * qy);
	R_[3] = 2.0 * (qy * qx + q0 * qz);
	R_[4] = 1.0 - 2.0 * (qx * qx + qz * qz);
	R_[5] = 2.0 * (qy * qz - q0 * qx);
	R_[6] = 2.0 * (qz * qx - q0 * qy);
	R_[7] = 2.0 * (qz * qy + q0 * qx);
	R_[8] = 1.0 - 2.0 * (qx * qx + qy * qy);

	double Sr = 0.0;
	double D = 0.0;
	double xri[3];
	double rxri[3];
	for (int i = 0; i < num_points; ++i)
	{
		Sr += X_r1[i].x * X_r1[i].x + X_r1[i].y * X_r1[i].y + X_r1[i].z * X_r1[i].z;
		xri[0] = X_r1[i].x;
		xri[1] = X_r1[i].y;
		xri[2] = X_r1[i].z;
		MatrixMuti1D(R_, 3, 3, xri, 3, 1, rxri);
		D += X_a1[i].x * rxri[0] + X_a1[i].y * rxri[1] + X_a1[i].z * rxri[2];
	}
	s_ = D / Sr;

	double RXr[3];
	MatrixMuti1D(R_, 3, 3, bar_X_r, 3, 1, RXr);
	t_[0] = bar_X_a[0] - s_ * RXr[0];
	t_[1] = bar_X_a[1] - s_ * RXr[1];
	t_[2] = bar_X_a[2] - s_ * RXr[2];

	cvReleaseMat(&cvE);
	cvReleaseMat(&cvN);
	cvReleaseMat(&cvEV);

	seven_parm_is_exist_ = true;
}

//对点从相对坐标系转到绝对定向
void CSevenParameter::CalculateRelativeToAbsolute(CoordDouble3D X_r, CoordDouble3D& X_a)
{
	//if (false == seven_parm_is_exist_)
	//{
	//	cout<<"Seven parameter is not calculated !"<<endl;
	//	exit(-1);
	//}
	double xr[3];
	xr[0] = X_r.x;
	xr[1] = X_r.y;
	xr[2] = X_r.z;

	InterfaceOsgAlgorithmFactory IAlgorithmFactory;
	InterfaceOsgAlgorithm* IAlgorithm = IAlgorithmFactory.create();

	double fwk[3] = {0};
	IAlgorithm->RtoAngle(R_, fwk[0], fwk[1], fwk[2]);

	double xr1[3];
	MatrixMuti1D(R_, 3, 3, xr, 3, 1, xr1);
	xr1[0] *= s_;
	xr1[1] *= s_;
	xr1[2] *= s_;

	X_a.x = xr1[0] + t_[0];
	X_a.y = xr1[1] + t_[1];
	X_a.z = xr1[2] + t_[2];
}

//输入相对定向的R_r和C_r
//输出绝对定向的R_a和C_a
//
////相对定向转绝对定向的R、C公式
////设七参数为Rs、ts、Ss
////则R'=R*Rs^(-1)
////C'=Ss*Rs*C + ts
////相机矩阵还是用回原来的K，即P'=K[R' | -R'C']
void CSevenParameter::CalculateAbsoluteRC(double* R_r, double* C_r, double* R_a, double* C_a)
{
	if (false == seven_parm_is_exist_)
	{
		cout<<"Seven parameter is not calculated !"<<endl;
		exit(-1);
	}

	double iR[9];
	InverseMatrix(R_, 3, iR);
	MatrixMuti1D(R_r, 3, 3, iR, 3, 3, R_a);

	double R_Cr[3];
	MatrixMuti1D(R_, 3, 3, C_r, 3, 1, R_Cr);
	C_a[0] = s_ * R_Cr[0] + t_[0];
	C_a[1] = s_ * R_Cr[1] + t_[1];
	C_a[2] = s_ * R_Cr[2] + t_[2];
}