#pragma once
#include "InterfaceOsgAlgorithm.h"

class OSGALGORITHMAPI osgAlgorithm : public InterfaceOsgAlgorithm
{
public:
	void CvmPrint(CvMat* M);

	void RtoAngle(double* R, double& phi, double& omega, double& kappa);

	void AngleToR(double f, double w, double k, double* R);

	bool PointInPolygon2D(double x, double y, const vector<CL::Vec2d> &POL);

	bool DoesPolygonIntersetPolygon(std::vector<CL::Vec2d> pol1, std::vector<CL::Vec2d> pol2);					

	bool FindMinMaxHeight(std::vector<osg::Vec3d> vecPt, double &minHeight, double &maxHeight);
};

