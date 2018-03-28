#pragma once
#include <vector>
#include "publicDefined.h"
#include "Geometry.h"
#include "VertsTriangle.h"

class CIsInAreaJudger
{
public:
	CIsInAreaJudger(std::vector<CScope> VecScope)
	{
		vecScope = VecScope;
	}
	
	virtual bool isPtInArea(osg::Vec3d pt);

	virtual bool isPtInScope(osg::Vec3d pt);

	virtual bool isTriangleInScope(CVertsTriangle vertsTri);

	virtual bool isTriangleInAbsoluteScope(CVertsTriangle vertsTri);

	virtual void setScope(std::vector<CScope> VecScope);

protected:
	virtual bool isTriangleInPolygon(CVertsTriangle vertsTri, std::vector<CL::Vec2d> pol, double minH, double maxH);

	virtual bool isTriangleInAbsolutePolygon(CVertsTriangle vertsTri, std::vector<CL::Vec2d> pol, double minH, double maxH);

	virtual std::vector<CL::Vec2d> obtainArea(int i);

private:
	std::vector<CScope> vecScope;

};

