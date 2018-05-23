#pragma once
#include "cv_plus.hpp"
#include "Geometry.h"

#ifdef OSGALGORITHMLIBDLL  
#define OSGALGORITHMAPI _declspec(dllexport)  
#else  
#define OSGALGORITHMAPI  _declspec(dllimport)  
#endif  


class OSGALGORITHMAPI InterfaceOsgAlgorithm
{
public:
	virtual void CvmPrint(CvMat* M) = 0;																	//打印矩阵

	virtual void RtoAngle(double* R, double& phi, double& omega, double& kappa) = 0;						//旋转矩阵变成角度

	virtual void AngleToR(double f, double w, double k, double* R) = 0;										//角度转旋转矩阵

	virtual bool PointInPolygon2D(double x, double y, const vector<CL::Vec2d> &POL) = 0;					//点在多边形内

	virtual bool DoesPolygonIntersetPolygon(std::vector<CL::Vec2d> pol1, std::vector<CL::Vec2d> pol2) = 0;	//面面是否相交

	virtual bool FindMinMaxHeight(std::vector<osg::Vec3d> vecPt, double &minHeight, double &maxHeight) = 0;	//查找最大最小高度
};


class OSGALGORITHMAPI InterfaceOsgAlgorithmFactory
{
public:
	InterfaceOsgAlgorithm* create();
};

