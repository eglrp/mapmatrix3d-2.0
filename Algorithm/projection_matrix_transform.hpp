#ifndef PROJECTION_MATIRX_TRANSFORM
#define PROJECTION_MATIRX_TRANSFORM

#include <iostream>
#include <vector>

#include <highgui.h>
#include <cv.h>
#include <cxcore.h>

#include "coord.hpp"
#include "my_matrix_compu.hpp"
#include "QR.hpp"

using namespace std;



#define MY_PI 3.1415926535897932384626433832795

namespace transMat
{
	//由计算机视觉的R矩阵分解出姿态角，注意先要做转置
	void RtoAngle(double* R, double& phi, double& omega, double& kappa);

	//姿态角转计算机视觉的旋转矩阵
	void AngleToR(double f, double w, double k, double* R);

	//由K、R、C计算出P矩阵
	void CalculateP(double* K, double* R, double* C, double* P);

	//P矩阵分解出K、R、C
	void CalculateKRC(double* P, double* K, double* R, double* C);

	//由焦距f，像主点，都是像素为单位，像主点以左上角为原点，生成K矩阵
	void CalculateK(double f, double x0, double y0, double* K);

	//六参数转换成P矩阵，直接转成以相片中心为原点的P矩阵
	void SixParmToP(double f, double x0, double y0, CoordDouble3D& xyz, CoordDouble3D& fwk, double* P);

	//由P矩阵和三维坐标计算二维坐标
	void Point3dToPoint2d(double* P, CoordDouble3D xyz, CoordDouble2D& xy);

	//由f和R、C计算ccd中心的坐标
	void CalculateCCDCenter(double f, double* R, double* C, CoordDouble3D& xyz_ccd_center);
}


#endif