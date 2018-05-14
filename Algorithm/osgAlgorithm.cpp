#include "StdAfx.h"
#include "osgAlgorithm.h"
#include "InterfaceOpencvDependency.h"
#include "projection_matrix_transform.hpp"

bool my_Greater(osg::Vec3d pt1, osg::Vec3d pt2)
{
	return pt1.z() < pt2.z();
}

void osgAlgorithm::CvmPrint(CvMat* M)
{
	InterfaceOpencvDependenyFactory IOpencvFactory;
	InterfaceOpencvDependeny* IOpencv = IOpencvFactory.create();
	IOpencv->OpencvCvmPrint(M);
}

void osgAlgorithm::RtoAngle(double* R, double& phi, double& omega, double& kappa)
{
	transMat::RtoAngle(R, phi, omega, kappa);
}

 void osgAlgorithm::AngleToR(double f, double w, double k, double* R) 
 {
	 transMat::AngleToR(f, w, k, R);
 }

bool osgAlgorithm::PointInPolygon2D(double x, double y, const vector<CL::Vec2d> &POL)
{
	return CL::Point_In_Polygon_2D(x, y, POL);
}

bool osgAlgorithm::DoesPolygonIntersetPolygon(std::vector<CL::Vec2d> pol1, std::vector<CL::Vec2d> pol2)
{
	return CL::IsPolygonIntersetPolygon(pol1, pol2);
}

bool osgAlgorithm::FindMinMaxHeight(std::vector<osg::Vec3d> vecPt, double &minHeight, double &maxHeight)
{
	std::sort(vecPt.begin(), vecPt.end(), my_Greater);
	int size = vecPt.size(); 

	if (size == 0)
	{
		return false;
	}

	minHeight = vecPt[0].z();
	maxHeight = vecPt[size - 1].z();

	return true;
}