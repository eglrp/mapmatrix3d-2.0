#pragma once
#include <vector>
#include "coord.hpp"
#include "InterfaceOsgAlgorithm.h"


class OSGALGORITHMAPI CSevenParameter
{
public:
	CSevenParameter();
	~CSevenParameter();

	void LoadControlPoints(std::vector<CoordDouble3D>& X_a, std::vector<CoordDouble3D>& X_r);
	//计算七参数
	void CalculateSevenParameter();
	//非像控点相对坐标转到绝对坐标
	void CalculateRelativeToAbsolute(CoordDouble3D X_r, CoordDouble3D& X_a);
	//由相对定向的P矩阵中的R和C，计算绝对定向的P矩阵中的R和C
	void CalculateAbsoluteRC(double* R_r, double* C_r, double* R_a, double* C_a);

	//七参数
	double R_[9];
	double t_[3];
	double s_;
	//像控点绝对坐标
	std::vector<CoordDouble3D> X_a_;
	//像控点相对坐标
	std::vector<CoordDouble3D> X_r_;
	//标示是否已经计算了七参数
	bool seven_parm_is_exist_;
};

